#pragma once
#include "N64System.h"
#include "N64Config.h"

namespace N64
{
	struct N64FrameInternalState
	{
		int currentField;
		int cpuEarnedCycles;
		int rspConsumableCycles;
	};

	class N64Frame
	{
	public:
		void Init(N64System& n64, const N64Config& config);
		void RunOnConsole(N64System& n64, const N64Config& config, const std::function<bool()>& breakPoint);
		void ControlFrame(N64System& n64, const N64Config& config);

	private:
		double m_fragmentTime{};

		N64FrameInternalState m_internalState{};
	};
}
