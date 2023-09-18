#pragma once
#include "SoftCommon.h"
#include "SoftEdgeWalker.h"

namespace N64::Rdp_detail::Soft
{
	template <IEdgeWalker Ew>
	void Rasterize(const CommanderContext& ctx, Ew ew, uint8 bpp);

	template void Rasterize<RectangleEdgeWalker>
	(const CommanderContext& ctx, RectangleEdgeWalker ew, uint8 bpp);

	template void Rasterize<TriangleEdgeWalker>
	(const CommanderContext& ctx, TriangleEdgeWalker ew, uint8 bpp);
}
