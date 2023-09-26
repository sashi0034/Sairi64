#include "SoftRenderer.h"

#include "N64/N64Logger.h"

namespace N64::Rdp_detail::Soft
{
	template <IEdgeWalker Ew>
	void renderNonShaded(const CommanderContext& ctx, const Ew& ew, uint8 bpp)
	{
		auto&& state = *ctx.state;
		const uint32 startY = ew.GetStartY();
		const uint32 endY = ew.GetEndY();
		for (uint32 y = startY; y <= endY; ++y)
		{
			const uint32 offset = state.colorImage.dramAddr + y * state.colorImage.width * bpp;

			const HSpan& horizontal = ew.GetXSpan(y);
			uint32 startX = horizontal.startX;
			uint32 endX = horizontal.endX;
			if (startX > endX) std::swap(startX, endX);

			for (uint32 x = startX * bpp; x < endX * bpp; x += bpp)
			{
				// TODO: サイクル0/1を考慮 (state.otherModes.cycleType)
				// TODO: color blend
				const Color32Bpp color = state.blendColor;
				switch (bpp)
				{
				case 2:
					WriteBytes16(ctx.rdram, EndianAddress<uint16>(offset + x), color.To16Bpp());
					break;
				case 4:
					WriteBytes32(ctx.rdram, EndianAddress<uint32>(offset + x), color);
					break;
				default: ;
				}
			}
		}
	}

	void RenderNonShadedTriangle(
		const CommanderContext& ctx, const TriangleEdgeWalker<CommandId::NonShadedTriangle>& ew, uint8 bpp)
	{
		renderNonShaded(ctx, ew, bpp);
	}

	// TODO: 統合
	void RenderShadeTextureTriangle(
		const CommanderContext& ctx,
		const TextureCoefficient& tc,
		const TriangleEdgeWalker<CommandId::ShadeTextureTriangle>& ew,
		uint8 bpp)
	{
		auto&& state = *ctx.state;
		const uint32 startY = ew.GetStartY();
		const uint32 endY = ew.GetEndY();
		const uint16 tmemBase = 0; // TODO?
		const auto dsDx = FixedPoint32<16, 16>(tc.DsDxInt(), tc.DsDxFrac());

		switch (state.textureImage.size)
		{
		// TODO: Px8 (used in OoT)
		case TexelSize::Px16:
			for (int y = startY; y <= endY; y++)
			{
				const uint32 screenLine = state.colorImage.dramAddr + state.colorImage.width * y * bpp;

				const HSpan& horizontal = ew.GetXSpan(y);
				uint32 startX = horizontal.startX;
				uint32 endX = horizontal.endX;
				if (startX > endX) std::swap(startX, endX);

				auto s = ew.GetAttr(y).s;
				auto t = ew.GetAttr(y).t;
				auto w = ew.GetAttr(y).w;
				// TODO: DsDy?
				// TODO: wの反映

				for (int x = startX; x <= endX; x++)
				{
					const auto processedT = t; // TODO?
					// const uint32 tmemXor = (processedT.Int() & 1) << 2;
					const uint16 tmemLine = tmemBase + processedT.Int() * bpp;

					const auto processedS = s; // TODO?
					const uint16 tmemAddr = ((tmemLine + processedS.Int() * bpp) & 0x7FF); // ^ tmemXor;
					const uint16 pixel = ReadTmem<uint16>(state, tmemAddr);
					WriteRdram<uint16>(ctx, screenLine + x * bpp, pixel);

					s += dsDx;
				}
			}
			break;
		default: break;
		}
	}

	void RenderFillRectangle(const CommanderContext& ctx, const RectangleEdgeWalker& ew, uint8 bpp)
	{
		renderNonShaded(ctx, ew, bpp);
	}
}
