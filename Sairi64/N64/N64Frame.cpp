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

	void emulateFrame(N64System& n64)
	{
		n64.GetCpu().Step(n64);
		n64.GetScheduler().Step();
	}

	void N64Frame::RunOnConsole(N64System& n64)
	{
		while (true)
		{
			emulateFrame(n64);
		}
	}

	void N64Frame::ControlFrame(N64System& n64)
	{
		const double actualDeltaTime = Scene::DeltaTime();
		constexpr double virtualDeltaTime = 1.0 / 60.0; // TODO: accurate FPS?

		m_fragmentTime += actualDeltaTime;

		// 60FPS制御
		while (m_fragmentTime >= virtualDeltaTime)
		{
			m_fragmentTime -= virtualDeltaTime;
			emulateFrame(n64);
		}
	}
}
