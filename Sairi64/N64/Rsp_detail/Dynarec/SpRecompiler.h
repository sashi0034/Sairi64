#pragma once
#include "ImemCache.h"

namespace N64::Rsp_detail::Dynarec
{
	struct SpRecompilingTarget
	{
		ImemAddr16 startPc;
		BlockCodeArray* destArray;
	};

	void SpRecompileFreshCode(N64System& n64, Rsp& rsp, const SpRecompilingTarget& target);
}
