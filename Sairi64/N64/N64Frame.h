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

	struct N64FrameInfo
	{
		uint64 frameCount;
		Error emulateError;
	};

	class N64Frame
	{
	public:
		void Init(N64System& n64, const N64Config& config);
		void RunOnConsole(N64System& n64, const N64Config& config, const std::function<bool()>& breakPoint);
		void ControlFrame(N64System& n64, const N64Config& config);
		const N64FrameInfo& Info() const { return m_info; }

	private:
		void runFrame(N64System& n64, const N64Config& config);

		double m_fragmentTime{};
		AsyncTask<void> m_frameTask{};

		N64FrameInternalState m_internalState{};
		N64FrameInfo m_info{};
	};
}
