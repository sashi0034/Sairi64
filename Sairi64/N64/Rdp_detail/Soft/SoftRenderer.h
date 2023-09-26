#pragma once
#include "SoftCommon.h"
#include "SoftEdgeWalker.h"

namespace N64::Rdp_detail::Soft
{
	void RenderNonShadedTriangle(
		const CommanderContext& ctx, const TriangleEdgeWalker<CommandId::NonShadedTriangle>& ew, uint8 bpp);

	void RenderShadeTextureTriangle(
		const CommanderContext& ctx,
		const TextureCoefficient& tc,
		const TriangleEdgeWalker<CommandId::ShadeTextureTriangle>& ew,
		uint8 bpp);

	void RenderFillRectangle(
		const CommanderContext& ctx, const RectangleEdgeWalker& ew, uint8 bpp);
}
