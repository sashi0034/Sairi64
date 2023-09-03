#pragma once
#include "ImemCache.h"

namespace N64::Rsp_detail::Dynarec
{
	struct RecompilingTarget
	{
		ImemAddr16 startPc;
		BlockCodeArray* destArray;
	};

	void RecompileFreshCode(N64System& n64, Rsp& rsp, const RecompilingTarget& target);
}
