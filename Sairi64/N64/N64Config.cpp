﻿#include "stdafx.h"
#include "N64Config.h"

namespace N64
{
	ProcessorType N64Config::defaultProcessor()
	{
		return ProcessorType::Dynarec;
	}
}
