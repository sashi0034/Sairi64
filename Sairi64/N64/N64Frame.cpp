#include "stdafx.h"
#include "N64Frame.h"

#include "Includes/imgui/imgui.h"

namespace N64
{
	void emulateFrame(N64System& sys)
	{
		sys.GetCpu().Step(sys);
	}

	void N64Frame::RunOnConsole(N64System& sys)
	{
		// ImGui::Begin("N64Frame StepCyclesCompressive");
		// ImGui::SliderInt("m_stepCycles", &m_stepCycles, 1, 100000);
		// ImGui::End();

		while (true)
		{
			emulateFrame(sys);
		}
	}

	void N64Frame::ControlFrame(N64System& sys)
	{
		const double actualDeltaTime = Scene::DeltaTime();
		constexpr double virtualDeltaTime = 1.0 / 60.0; // TODO: accurate FPS?

		m_fragmentTime += actualDeltaTime;

		// 60FPS制御
		while (m_fragmentTime >= virtualDeltaTime)
		{
			m_fragmentTime -= virtualDeltaTime;
			emulateFrame(sys);
		}
	}
}
