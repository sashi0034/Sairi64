#pragma once
#include "N64/Forward.h"
#include "RecompiledCache.h"

namespace N64::Cpu_detail::Dynarec
{
	RecompiledResult RecompileFreshCode(N64System& n64, Cpu& cpu, PAddr32 startPc);
}
