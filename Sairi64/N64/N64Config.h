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

	private:
		static ProcessorType defaultProcessor();
	};
}
