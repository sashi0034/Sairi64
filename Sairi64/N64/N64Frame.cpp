#include "stdafx.h"
#include "N64Frame.h"

namespace N64
{
	void emulateFrame(N64System& sys)
	{
		sys.GetCpu().Step(sys);
	}

	void N64Frame::RunUntilAbort(N64System& sys)
	{
		while (true)
		{
			sys.GetCpu().Step(sys);
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
