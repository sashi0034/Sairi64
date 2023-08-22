#pragma once
#include "N64System.h"

namespace N64
{
	struct N64InitArgs
	{
		String filePath;
	};

	struct N64FrameInternalState
	{
		int rspConsumableCycles;
	};

	class N64Frame
	{
	public:
		void Init(N64System& n64, const N64InitArgs& arg);
		void RunOnConsole(N64System& n64);
		void ControlFrame(N64System& n64);

	private:
		double m_fragmentTime{};

		N64FrameInternalState m_internalState{};
	};
}
