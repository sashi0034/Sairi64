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

		ProcessorType processor = ProcessorType::Dynarec;

		struct
		{
			bool executePifRom = true;
		} boot;
	};
}
