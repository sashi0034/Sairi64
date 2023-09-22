#pragma once
#include "SoftCommon.h"

namespace N64::Rdp_detail::Soft
{
	template <TexelSize textureImageSize>
	static void LoaderTile(const CommanderContext& ctx, const RdpCommand& cmd)
	{
		auto&& state = *ctx.state;
		const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());
		auto&& descriptor = state.tiles[tileIndex];

		const FixedPoint16<14, 2> sl = GetBits<44, 55>(cmd.Data<0>());
		const FixedPoint16<14, 2> tl = GetBits<32, 43>(cmd.Data<0>());
		const FixedPoint16<14, 2> sh = GetBits<12, 23>(cmd.Data<0>());
		const FixedPoint16<14, 2> th = GetBits<0, 11>(cmd.Data<0>());

		const uint32 bytesPerTextureLine =
			GetBytesPerTextureLine(textureImageSize, state.textureImage.width);
		const uint32 bytesPerTileLine = descriptor.line * sizeof(uint64);

		const uint32 tmemBase = descriptor.tmemAddr * sizeof(uint64);
		const uint32 dramBase = state.textureImage.dramAddr;
		const uint8 bytesPerTexel = GetBytesParTexel(textureImageSize);

		for (uint32 t = 0; t <= (th.Int() - tl.Int()); t++)
		{
			const uint32 tileLine = tmemBase + (t + tl.Int()) * bytesPerTileLine + sl.Int() * bytesPerTexel;
			const uint32 dramLine = dramBase + (t + tl.Int()) * bytesPerTextureLine + sl.Int() * bytesPerTexel;
			// const uint32 tmemXor = t & 1 ? 4 : 0;
			for (uint32 s = 0; s <= (sh.Int() - sl.Int()); s++)
			{
				const uint32 dramTexelAddress = dramLine + s * bytesPerTexel;

				uint16 tmemTexelAddress = tileLine + s * bytesPerTexel;
				// tmemTexelAddress ^= tmemXor;
				tmemTexelAddress &= 0x7FF;

				if constexpr (textureImageSize == TexelSize::Px8)
				{
					const uint8 texel = ReadRdram<uint8>(ctx, dramTexelAddress);
					WriteTmem<uint8>(state, tmemTexelAddress, texel);
				}
				else if constexpr (textureImageSize == TexelSize::Px16)
				{
					const uint16 texel = ReadRdram<uint16>(ctx, dramTexelAddress);
					WriteTmem<uint16>(state, tmemTexelAddress, texel);
				}
				else if constexpr (textureImageSize == TexelSize::Px32)
				{
					const uint32 texel = ReadRdram<uint32>(ctx, dramTexelAddress);
					WriteTmem<uint32>(state, tmemTexelAddress, texel);
				}
				else static_assert(AlwaysFalseValue<TexelSize, textureImageSize>);
			}
		}
	}
}
