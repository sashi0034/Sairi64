#pragma once

#include "SoftCommon.h"
#include "SoftEdgeWalker.h"
#include "SoftRasterizer.h"

// https://ultra64.ca/files/documentation/silicon-graphics/SGI_RDP_Command_Summary.pdf

namespace N64::Rdp_detail::Soft
{
	class Soft
	{
	public:
		[[nodiscard]]
		static SoftUnit NonShadedTriangle(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			static TriangleEdgeWalker edgeWalker{};
			const auto ec = static_cast<EdgeCoefficient<0>>(cmd);
			auto&& state = *ctx.state;
			const uint8 bpp = GetBytesPerPixel(state);

			edgeWalker.EdgeWalk(ec, bpp);
			Rasterize(ctx, edgeWalker, bpp);

			return {};
		}

		// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L1031
		[[nodiscard]]
		static SoftUnit FillRectangle(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& state = *ctx.state;
			const int xl = GetBits<44, 55>(cmd.Data<0>()) >> 2;
			const int yl = GetBits<32, 43>(cmd.Data<0>()) >> 2;
			const int xh = GetBits<12, 23>(cmd.Data<0>()) >> 2;
			const int yh = GetBits<0, 11>(cmd.Data<0>()) >> 2;

			const int bytesPerPixel = GetBytesPerPixel(state);

			const int startX = xh;
			const int endX = (xl + 1);

			const auto edgeWalker = RectangleEdgeWalker(yh, yl, HSpan{startX, endX});
			Rasterize(ctx, edgeWalker, bytesPerPixel);

			return {};
		}

		[[nodiscard]]
		static SoftUnit LoadBlock(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& state = *ctx.state;
			const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());
			auto&& tile = state.tiles[tileIndex];
			const uint16 sl = GetBits<44, 55>(cmd.Data<0>());
			const uint16 tl = GetBits<32, 43>(cmd.Data<0>());
			const uint16 sh = GetBits<12, 23>(cmd.Data<0>());
			const uint16 dxt = GetBits<0, 11>(cmd.Data<0>());
			tile.sl = sl;
			tile.tl = tl;

			const uint32 tmemBase = tile.tmemAddr * sizeof(uint64);
			const uint32 dramBase = state.textureImage.dramAddr;
			switch (state.textureImage.size)
			{
			case TexelSize::Px16: {
				constexpr uint8 bytesPerTexel = 2;
				for (uint32 s = sl; s <= sh; s++)
				{
					// TODO: DxTをもとに奇数行のスワッピング
					const uint32 dramTexelAddress = dramBase + s * bytesPerTexel;
					const uint16 texel = ReadRdram<uint16>(ctx, dramTexelAddress);

					const uint16 tmemTexelAddress = (tmemBase + (s * bytesPerTexel)) & TmemSizeMask_0xFFF;
					WriteTmem<uint16>(state, tmemTexelAddress, texel);
				}
				break;
			}
			default:
				N64Logger::Abort();
			}
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetScissor(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& scissor = ctx.state->scissorRect;
			scissor.yl = GetBits<0, 11>(cmd.Data<0>());
			scissor.xl = GetBits<12, 23>(cmd.Data<0>());
			scissor.yh = GetBits<32, 43>(cmd.Data<0>());
			scissor.xh = GetBits<44, 55>(cmd.Data<0>());
			scissor.f = GetBits<25>(cmd.Data<0>());
			scissor.o = GetBits<24>(cmd.Data<0>());
			return {};
		}

		// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L806
		[[nodiscard]]
		static SoftUnit SetOtherModes(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& otherModes = ctx.state->otherModes;
			otherModes.atomicPrim = GetBits<55>(cmd.Data<0>());
			otherModes.cycleType = GetBits<52, 53>(cmd.Data<0>());
			otherModes.perspTexEn = GetBits<51>(cmd.Data<0>());
			otherModes.detailTexEn = GetBits<50>(cmd.Data<0>());
			otherModes.sharpenTexEn = GetBits<49>(cmd.Data<0>());
			otherModes.texLodEn = GetBits<48>(cmd.Data<0>());
			otherModes.enTlut = GetBits<47>(cmd.Data<0>());
			otherModes.tlutType = GetBits<46>(cmd.Data<0>());
			otherModes.sampleType = GetBits<45>(cmd.Data<0>());
			otherModes.midTexel = GetBits<44>(cmd.Data<0>());
			otherModes.biLerp0 = GetBits<43>(cmd.Data<0>());
			otherModes.biLerp1 = GetBits<42>(cmd.Data<0>());
			otherModes.convertOne = GetBits<41>(cmd.Data<0>());
			otherModes.keyEn = GetBits<40>(cmd.Data<0>());
			otherModes.rgbDitherSel = GetBits<38, 39>(cmd.Data<0>());
			otherModes.alphaDitherSel = GetBits<36, 37>(cmd.Data<0>());

			otherModes.blenderConfig0.source1a = From1a(GetBits<30, 31>(cmd.Data<0>()));
			otherModes.blenderConfig1.source1a = From1a(GetBits<28, 29>(cmd.Data<0>()));

			otherModes.blenderConfig0.source1b = From1a(GetBits<26, 27>(cmd.Data<0>()));
			otherModes.blenderConfig1.source1b = From1a(GetBits<24, 25>(cmd.Data<0>()));

			otherModes.blenderConfig0.source2a = From1a(GetBits<22, 23>(cmd.Data<0>()));
			otherModes.blenderConfig1.source2a = From1a(GetBits<20, 21>(cmd.Data<0>()));

			otherModes.blenderConfig0.source2b = From1a(GetBits<18, 19>(cmd.Data<0>()));
			otherModes.blenderConfig1.source2b = From1a(GetBits<24, 25>(cmd.Data<0>()));

			otherModes.forceBlend = GetBits<14>(cmd.Data<0>());
			otherModes.alphaCvgSelect = GetBits<13>(cmd.Data<0>());
			otherModes.cvgTimesAlpha = GetBits<12>(cmd.Data<0>());
			otherModes.zMode = GetBits<10, 11>(cmd.Data<0>());
			otherModes.cvgDest = GetBits<8, 9>(cmd.Data<0>()); // TODO: <9, 8>になるようにしないと駄目かも (3 - (b ^ 3))
			otherModes.colorOnCvg = GetBits<7>(cmd.Data<0>());
			otherModes.imageReadEn = GetBits<6>(cmd.Data<0>());
			otherModes.zUpdateEn = GetBits<5>(cmd.Data<0>());
			otherModes.zCompareEn = GetBits<4>(cmd.Data<0>());
			otherModes.antialiasEn = GetBits<3>(cmd.Data<0>());
			otherModes.zSourceSel = GetBits<2>(cmd.Data<0>());
			otherModes.ditherAlphaEn = GetBits<1>(cmd.Data<0>());
			otherModes.alphaCompareEn = GetBits<0>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetTile(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());
			auto&& tile = ctx.state->tiles[tileIndex];
			tile.format = GetBits<53, 55>(cmd.Data<0>());
			tile.size = GetBits<51, 52>(cmd.Data<0>());
			tile.line = GetBits<41, 49>(cmd.Data<0>());
			tile.tmemAddr = GetBits<32, 40>(cmd.Data<0>());
			tile.palette = GetBits<20, 23>(cmd.Data<0>());
			tile.ct = GetBits<19>(cmd.Data<0>());
			tile.mt = GetBits<18>(cmd.Data<0>());
			tile.maskT = GetBits<14, 17>(cmd.Data<0>());
			tile.shiftT = GetBits<10, 13>(cmd.Data<0>());
			tile.cs = GetBits<9>(cmd.Data<0>());
			tile.ms = GetBits<8>(cmd.Data<0>());
			tile.maskS = GetBits<4, 7>(cmd.Data<0>());
			tile.shiftS = GetBits<0, 3>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetTileSize(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());
			auto&& tile = ctx.state->tiles[tileIndex];
			tile.sl = GetBits<44, 55>(cmd.Data<0>());
			tile.tl = GetBits<32, 43>(cmd.Data<0>());
			tile.sh = GetBits<12, 23>(cmd.Data<0>());
			tile.th = GetBits<0, 11>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetFillColor(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			ctx.state->fillColor = GetBits<0, 31>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetBlendColor(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			ctx.state->blendColor = cmd.Data<0>();
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetCombine(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& combine = ctx.state->combine;
			combine.subA_Rgb0 = GetBits<52, 55>(cmd.Data<0>());
			combine.mulRgb0 = GetBits<47, 51>(cmd.Data<0>());
			combine.subA_Alpha0 = GetBits<44, 46>(cmd.Data<0>());
			combine.mulAlpha0 = GetBits<41, 43>(cmd.Data<0>());
			combine.subA_Rgb1 = GetBits<37, 40>(cmd.Data<0>());
			combine.mulRgb1 = GetBits<32, 36>(cmd.Data<0>());
			combine.subB_Rgb0 = GetBits<28, 31>(cmd.Data<0>());
			combine.subB_Rgb1 = GetBits<24, 27>(cmd.Data<0>());
			combine.subA_Alpha1 = GetBits<21, 23>(cmd.Data<0>());
			combine.mulAlpha1 = GetBits<18, 20>(cmd.Data<0>());
			combine.addRgb0 = GetBits<15, 17>(cmd.Data<0>());
			combine.subB_Alpha0 = GetBits<12, 14>(cmd.Data<0>());
			combine.addAlpha0 = GetBits<9, 11>(cmd.Data<0>());
			combine.addRgb1 = GetBits<6, 8>(cmd.Data<0>());
			combine.subB_Alpha1 = GetBits<3, 5>(cmd.Data<0>());
			combine.addAlpha1 = GetBits<0, 2>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetColorImage(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& colorImage = ctx.state->colorImage;
			colorImage.format = GetBits<53, 55>(cmd.Data<0>());
			colorImage.size = GetBits<51, 52>(cmd.Data<0>());
			colorImage.width = GetBits<32, 41>(cmd.Data<0>()) + 1;
			colorImage.dramAddr = GetBits<0, 25>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetTextureImage(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& textureImage = ctx.state->textureImage;
			textureImage.format = GetBits<53, 55>(cmd.Data<0>());
			textureImage.size = static_cast<TexelSize>(GetBits<51, 52>(cmd.Data<0>()));
			textureImage.width = GetBits<32, 41>(cmd.Data<0>()) + 1;
			textureImage.dramAddr = GetBits<0, 25>(cmd.Data<0>());
			return {};
		}

		[[nodiscard]]
		static SoftUnit SetMaskImage(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			ctx.state->zImage = GetBits<0, 25>(cmd.Data<0>());
			return {};
		}
	};
}
