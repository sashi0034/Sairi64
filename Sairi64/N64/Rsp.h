#pragma once
#include "Rsp_detail/Rsp.h"

namespace N64
{
	using Rsp = Rsp_detail::Rsp;
	using ImemAddr16 = Rsp_detail::ImemAddr16;
	constexpr auto SpMemoryMask_0xFFF = Rsp_detail::SpMemoryMask_0xFFF;
}
