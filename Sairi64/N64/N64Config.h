#pragma once
#include "Forward.h"

namespace N64
{
	struct N64Config
	{
		struct
		{
			String filePath;
		} rom;

		ProcessorType processor = defaultProcessor();

		struct
		{
			bool executePifRom = true;
		} boot;

		bool threadingRun = defaultThreadingRun();

	private:
		static ProcessorType defaultProcessor();
		static bool defaultThreadingRun();
	};
}
