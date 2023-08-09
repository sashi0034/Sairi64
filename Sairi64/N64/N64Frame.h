#pragma once
#include "N64System.h"

namespace N64
{
	class N64Frame
	{
	public:
		void RunOnConsole(N64System& sys);
		void ControlFrame(N64System& sys);

	private:
		double m_fragmentTime{};
	};
}
