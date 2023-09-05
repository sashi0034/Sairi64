#include "stdafx.h"
#include "N64Frame.h"

#include "N64Logger.h"
#include "Mmio/Pif.h"

namespace N64
{
	void N64Frame::Init(N64System& n64, const N64Config& config)
	{
		n64.GetMemory().GetRom().LoadFile(config.rom.filePath);

		if (config.boot.executePifRom) Mmio::Pif::ExecuteRom(n64);
	}

	template <bool hasBreakPoint, ProcessorType processor>
	[[nodiscard]] inline bool emulateFrame_stepCyclesPerHalfLine(
		N64System& n64, N64FrameInternalState& state, const std::function<bool()>& breakPoint = {})
	{
		const int cyclesPerHalfLine = n64.GetVI().CyclesPerHalfLine();
		while (true)
		{
			// CPUステップ
			const CpuCycles taken = n64.GetCpu().Step<processor>(n64);

			// RSPステップ
			if (n64.GetRsp().IsHalted() == false)
			{
				state.rspConsumableCycles += taken * 2;
				while (state.rspConsumableCycles >= 3)
				{
					// CPUステップ3回につき、RSPステップ2回になるように
					state.rspConsumableCycles -= 3;
					n64.GetRsp().Step(n64);
				}
			}
			else
			{
				state.rspConsumableCycles = 0;
			}

			// スケジューラステップ
			n64.GetScheduler().Step(taken);

			if constexpr (hasBreakPoint)
			{
				if (breakPoint()) return true;
			}

			N64_TRACE(U"end current step: {}\n"_fmt(taken));

			// 終了チェック
			state.cpuEarnedCycles += taken;
			if (state.cpuEarnedCycles >= cyclesPerHalfLine) break;
		}
		// 1ライン終了
		state.cpuEarnedCycles -= cyclesPerHalfLine;

		return false;
	}

	template <bool hasBreakPoint = false, ProcessorType processor>
	bool emulateFrame(N64System& n64, N64FrameInternalState& state, const std::function<bool()>& breakPoint = {})
	{
		auto&& vi = n64.GetVI();

		for (int line = 0; line < vi.NumHalfLines(); ++line)
		{
			// VCurrent=VInterruptのとき割り込み発生
			const int actualLine = (line << 1) | state.currentField; // 現在の描画対象のライン
			vi.SetVCurrent(actualLine);
			if ((vi.VCurrent() & 0x3FE) == vi.VInterrupt())
				InterruptRaise<Interruption::VI>(n64);

			// CPUやRSPなど実行
			if (hasBreakPoint)
			{
				// ブレイクポイントに引っかかったら終了 (デバッグ用)
				if (emulateFrame_stepCyclesPerHalfLine<hasBreakPoint, processor>(n64, state, breakPoint))
					return true;
			}
			else
			{
				// 通常
				(void)emulateFrame_stepCyclesPerHalfLine<hasBreakPoint, processor>(n64, state);
			}
		}

		// fieldが0のときは上から下へ通常描画、1のときは1ラインずつ開けながら描画される
		state.currentField = (state.currentField + 1) % vi.NumFields();

		// 全/半ライン描画完了時にも割り込みチェック (?)
		if ((vi.VCurrent() & 0x3FE) == vi.VInterrupt())
			InterruptRaise<Interruption::VI>(n64);

		// 画面更新
		n64.GetRdp().UpdateDisplay(n64);

		return false;
	}

	template <bool hasBreakPoint = false>
	bool emulateFrame(
		N64System& n64,
		N64FrameInternalState& state,
		ProcessorType processor,
		const std::function<bool()>& breakPoint = {})
	{
		if (processor == ProcessorType::Interpreter)
			return emulateFrame<hasBreakPoint, ProcessorType::Interpreter>(n64, state, breakPoint);
		else if (processor == ProcessorType::Dynarec)
			return emulateFrame<hasBreakPoint, ProcessorType::Dynarec>(n64, state, breakPoint);
		else return false;
	}

	void N64Frame::RunOnConsole(N64System& n64, const N64Config& config, const std::function<bool()>& breakPoint)
	{
		while (true)
		{
			const bool breaking = emulateFrame<true>(n64, m_internalState, config.processor, breakPoint);
			if (breaking) return;
		}
	}

	void N64Frame::ControlFrame(N64System& n64, const N64Config& config)
	{
		const double actualDeltaTime = Scene::DeltaTime();
		const double virtualDeltaTime = 1.0 / GetFps_60_50(n64.GetMemory().IsRomPal());

		m_fragmentTime += actualDeltaTime;

		// 60FPS制御
		while (m_fragmentTime >= virtualDeltaTime)
		{
			m_fragmentTime -= virtualDeltaTime;
			emulateFrame(n64, m_internalState, config.processor);
		}
	}
}
