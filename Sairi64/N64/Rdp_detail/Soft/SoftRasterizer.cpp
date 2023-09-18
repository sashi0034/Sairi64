#include "SoftRasterizer.h"

namespace N64::Rdp_detail::Soft
{
	template <IEdgeWalker Ew>
	void Rasterize(const CommanderContext& ctx, Ew ew, uint8 bpp)
	{
		auto&& state = *ctx.state;
		const uint32 startY = ew.GetStartY();
		const uint32 endY = ew.GetEndY();
		for (uint32 y = startY; y <= endY; ++y)
		{
			const uint32 offset = state.colorImage.dramAddr + y * state.colorImage.width * bpp;

			const HSpan& horizontal = ew.GetHSpan(y);
			uint32 startX = horizontal.startX;
			uint32 endX = horizontal.endX;
			if (startX > endX) std::swap(startX, endX);

			for (uint32 x = startX * bpp; x < endX * bpp; x += bpp)
			{
				// TODO: サイクル0/1を考慮 (state.otherModes.cycleType)
				// TODO: color blend
				switch (bpp)
				{
				case 2: {
					const Color16Bpp color = 0xFFFF'FFFF;
					WriteBytes16(ctx.rdram, EndianAddress<uint16>(offset + x), color);
				}
				case 4: {
					const Color32Bpp color = 0xFFFF'FFFF'FFFF'FFFF;
					WriteBytes32(ctx.rdram, EndianAddress<uint32>(offset + x), color);
				}
				default: ;
				}
			}
		}
	}
}
