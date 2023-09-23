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

		struct
		{
			double volume;
		} audio;

		ProcessorType processor = defaultProcessor();

		struct
		{
			bool executePifRom = true;
		} boot;

		bool threadingRun = defaultThreadingRun();

		static N64Config LoadToml(const FilePath& path, const FilePath& alternativePath);

	private:
		static ProcessorType defaultProcessor();
		static bool defaultThreadingRun();
	};
}
