#include "stdafx.h"
#include "N64Config.h"

namespace N64
{
	N64Config N64Config::LoadToml(const FilePath& path, const FilePath& alternativePath)
	{
		TOMLReader toml{path};
		if (not toml)
		{
			toml = TOMLReader(alternativePath);
		}

		auto config = N64Config();
		config.rom.filePath = toml[U"rom.filepath"].getString();

		return config;
	}

	ProcessorType N64Config::defaultProcessor()
	{
		return ProcessorType::Dynarec;
	}

	bool N64Config::defaultThreadingRun()
	{
		return true;
	}
}
