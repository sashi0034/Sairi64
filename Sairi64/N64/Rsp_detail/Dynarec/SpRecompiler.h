#pragma once
#include "ImemCache.h"

namespace N64::Rsp_detail::Dynarec
{
	SpRecompileResult SpRecompileFreshCode(N64System& n64, Rsp& rsp, ImemAddr16 startPc);
}
