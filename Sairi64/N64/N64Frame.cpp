#include "stdafx.h"
#include "N64Frame.h"

#include "Mmio/Pif.h"
#include "Includes/imgui/imgui.h"

namespace N64
{
	void N64Frame::Init(N64System& n64, const N64InitArgs& arg)
	{
		n64.GetMemory().GetRom().LoadFile(arg.filePath);

		Mmio::Pif::ExecuteRom(n64);
	}

	void emulateFrame_stepCyclesPerHalfLine(N64System& n64, N64FrameInternalState& state)
	{
		for (int i = 0; i < n64.GetVI().CyclesPerHalfLine(); ++i)
		{
			// CPUステップ
			n64.GetCpu().Step(n64);

			if (state.rspConsumableCycles++; state.rspConsumableCycles >= 3)
			{
				// CPUステップ3回につき、RSPステップ2回
				state.rspConsumableCycles -= 3;
				n64.GetRsp().Step(n64);
			}

			// スケジューラステップ
			n64.GetScheduler().Step();
		}
	}

	void emulateFrame(N64System& n64, N64FrameInternalState& state)
	{
		auto&& vi = n64.GetVI();

		// fieldが1のときは上から下へ通常描画、2のときは1ラインずつ開けながら描画される
		for (int field = 0; field < vi.NumFields(); ++field)
		{
			for (int line = 0; line < vi.NumHalfLines(); ++line)
			{
				// VCurrent=VInterruptのとき割り込み発生
				const int actualLine = (line << 1) | field; // 現在の描画対象のライン
				vi.SetVCurrent(actualLine);
				if (vi.VCurrent() & 0x3FE == vi.VInterrupt())
					InterruptRaise<Interruption::VI>(n64);

				// CPUやRSPなど実行
				emulateFrame_stepCyclesPerHalfLine(n64, state);
			}

			// 全/半ライン描画完了時にも割り込みチェック (?)
			if (vi.VCurrent() & 0x3FE == vi.VInterrupt())
				InterruptRaise<Interruption::VI>(n64);

			// 画面更新
			n64.GetRdp().UpdateDisplay(n64);
		}
	}

	void N64Frame::RunOnConsole(N64System& n64)
	{
		while (true)
		{
			emulateFrame(n64, m_internalState);
		}
	}

	void N64Frame::ControlFrame(N64System& n64)
	{
		const double actualDeltaTime = Scene::DeltaTime();
		const double virtualDeltaTime = 1.0 / GetFps_60_50(n64.GetMemory().IsRomPal());

		m_fragmentTime += actualDeltaTime;

		// 60FPS制御
		while (m_fragmentTime >= virtualDeltaTime)
		{
			m_fragmentTime -= virtualDeltaTime;
			emulateFrame(n64, m_internalState);
		}
	}
}
