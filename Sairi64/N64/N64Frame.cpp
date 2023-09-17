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
			const CpuCycles takenCpu = n64.GetCpu().Step<processor>(n64);

			// RSPステップ
			if (n64.GetRsp().IsHalted() == false)
			{
				state.rspConsumableCycles += takenCpu * 2;
				while (state.rspConsumableCycles >= 3)
				{
					// CPUステップ3回につき、RSPステップ2回になるように
					const uint32 takenRsp = n64.GetRsp().Step(n64);
					state.rspConsumableCycles -= takenRsp * 3;
				}
			}
			else
			{
				state.rspConsumableCycles = 0;
			}

			// スケジューラステップ
			n64.GetScheduler().Step(takenCpu);

			if constexpr (hasBreakPoint)
			{
				if (breakPoint()) return true;
			}

			N64_TRACE(General, U"end current step: {}\n"_fmt(takenCpu));

			// 終了チェック
			state.cpuEarnedCycles += takenCpu;
			if (state.cpuEarnedCycles >= cyclesPerHalfLine) break;
		}
		// 1ライン終了
		state.cpuEarnedCycles -= cyclesPerHalfLine;

		// 音響ステップ
		state.audioConsumableCycles += cyclesPerHalfLine;
		while (state.audioConsumableCycles > 0)
		{
			const uint32 takenAudio = n64.GetAI().Step(n64);
			state.audioConsumableCycles -= takenAudio;
		}

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

			if ((vi.VCurrent() & 0x3FE) == vi.VInterrupt())
				InterruptRaise<Interruption::VI>(n64);
		}

		// fieldが0のときは上から下へ通常描画、1のときは1ラインずつ開けながら描画される
		state.currentField = (state.currentField + 1) % vi.NumFields();

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

	void profileFrame(Stopwatch& stopwatch, uint64* profilingCount, double* frameRate)
	{
		if (stopwatch.isStarted() == false || stopwatch.isPaused())
		{
			stopwatch.start();
		}
		else if (stopwatch.sF() >= 1.0)
		{
			*profilingCount += 1;
			*frameRate = static_cast<double>(*profilingCount) / stopwatch.sF();
			*profilingCount = 0;
			stopwatch.restart();
		}
		else
		{
			*profilingCount += 1;
		}
	}

	inline bool checkSuspend(bool isSuspended, Stopwatch& stopwatch)
	{
		if (isSuspended)
		{
			stopwatch.pause();
		}
		return isSuspended;
	}

	void N64Frame::runFrame(N64System& n64, const N64Config& config)
	{
		const double virtualDeltaTime = 1.0 / GetFps_60_50(n64.GetMemory().IsRomPal());
		try
		{
			while (m_fragmentTime >= virtualDeltaTime)
			{
				// 実行
				m_fragmentTime -= virtualDeltaTime;
				m_info.frameCount++;
				emulateFrame(n64, m_internalState, config.processor);
				profileFrame(m_profilingStopwatch, &m_profilingCount, &m_info.frameRate);
				if (checkSuspend(m_isSuspended, m_profilingStopwatch)) break;
			}
		}
		catch (const Error& e)
		{
			m_info.emulateError = std::move(e);
		}
	}

	void N64Frame::StepSingleFrame(N64System& n64, const N64Config& config)
	{
		// ステップ実行
		m_info.frameCount++;
		emulateFrame(n64, m_internalState, config.processor);
	}

	void N64Frame::checkHandleError()
	{
		if (m_isHandledError == false)
		{
			m_isHandledError = true;
			System::MessageBoxOK(U"Error!\n" + m_info.emulateError.what(), MessageBoxStyle::Error);
		}
	}

	void N64Frame::ControlFrame(N64System& n64, const N64Config& config)
	{
		if (m_info.emulateError.what().isEmpty() == false)
		{
			checkHandleError();
			return;
		}
		if (checkSuspend(m_isSuspended, m_profilingStopwatch)) return;;

		const double actualDeltaTime = Scene::DeltaTime();

		m_fragmentTime += actualDeltaTime;

		if (m_frameTask.isValid() && m_frameTask.isReady() == false) return;

		// 60FPS制御
		if (config.threadingRun == false)
		{
			// シングルスレッド処理
			runFrame(n64, config);
		}
		else
		{
			// 別スレッド処理
			m_frameTask = Async([&n64, this, &config]()
			{
				runFrame(n64, config);
			});
		}
	}
}
