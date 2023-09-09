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
		double frameRate;
		Error emulateError;
	};

	class N64Frame
	{
	public:
		void Init(N64System& n64, const N64Config& config);
		void RunOnConsole(N64System& n64, const N64Config& config, const std::function<bool()>& breakPoint);
		void ControlFrame(N64System& n64, const N64Config& config);
		void StepSingleFrame(N64System& n64, const N64Config& config);
		const N64FrameInfo& Info() const { return m_info; }

		void SetSuspend(bool isSuspended) { m_isSuspended = isSuspended; }
		bool IsSuspended() const { return m_isSuspended; }

	private:
		void runFrame(N64System& n64, const N64Config& config);
		void checkHandleError();

		bool m_isSuspended{};
		double m_fragmentTime{};
		AsyncTask<void> m_frameTask{};
		Stopwatch m_profilingStopwatch{};
		uint64 m_profilingCount{};
		bool m_isHandledError{};

		N64FrameInternalState m_internalState{};
		N64FrameInfo m_info{};
	};
}
