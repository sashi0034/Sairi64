#pragma once
#include "N64System.h"

namespace N64
{
	class N64Frame
	{
	public:
		void RunUntilAbort(N64System& sys, int cycleCount = -1);
		void ControlFrame(N64System& sys);
	private:
		double m_fragmentTime{};
	};
}
