#pragma once

#include "SoftCommon.h"
#include "SoftEdgeWalker.h"
#include "SoftLoader.h"
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
		static SoftUnit TextureRectangle(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& state = *ctx.state;

			const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());

			const FixedPoint16<14, 2> xl = GetBits<44, 55>(cmd.Data<0>());
			const FixedPoint16<14, 2> yl = GetBits<32, 43>(cmd.Data<0>());
			const FixedPoint16<14, 2> xh = GetBits<12, 23>(cmd.Data<0>());
			const FixedPoint16<14, 2> yh = GetBits<0, 11>(cmd.Data<0>());

			const FixedPoint16<11, 5> startS = GetBits<48, 63>(cmd.Data<1>());
			const FixedPoint16<11, 5> startT = GetBits<32, 47>(cmd.Data<1>());
			const FixedPoint16<6, 10> dsDx = state.otherModes.cycleType == CycleType::Copy
			                                 ? GetBits<16, 31>(cmd.Data<1>()) >> 2
			                                 : GetBits<16, 31>(cmd.Data<1>()); // ?
			const FixedPoint16<6, 10> dtDy = GetBits<0, 15>(cmd.Data<1>());

			const auto& descriptor = state.tiles[tileIndex];
			const uint16 tmemBase = descriptor.tmemAddr * sizeof(uint64);

			const uint8 bytesPerPixel = GetBytesPerPixel(state);

			const uint32 bytesPerScreenLine = state.colorImage.width * bytesPerPixel;
			const uint32 bytesPerTileLine = descriptor.line * sizeof(uint64);
			const uint8 bytesPerTexel = GetBytesParTexel(descriptor.size);

			auto s = startS;
			auto t = startT;
			switch (descriptor.size)
			{
			case TexelSize::Px16:
				for (int y = yh.Int(); y <= yl.Int(); y++)
				{
					const uint32 screenLine = state.colorImage.dramAddr + y * bytesPerScreenLine;
					for (int x = xh.Int(); x <= xl.Int(); x++)
					{
						const auto processedT =
							ProcessST(t, descriptor.ct, descriptor.mt, descriptor.maskT, descriptor.shiftT);
						// const uint32 tmemXor = (processedT.Int() & 1) << 2;
						const uint16 tmemLine = tmemBase + processedT.Int() * bytesPerTileLine;

						const auto processedS = s;
						// TODO: 直す
						// ProcessST(s, descriptor.cs, descriptor.ms, descriptor.maskS, descriptor.shiftS);
						const uint16 tmemAddr = ((tmemLine + processedS.Int() * bytesPerTexel) & 0x7FF); // ^ tmemXor;
						const uint16 pixel = ReadTmem<uint16>(state, tmemAddr);
						WriteRdram<uint16>(ctx, screenLine + x * bytesPerPixel, pixel);

						s += dsDx;
					}
					t += dtDy;
					s = startS;
				}
				break;
			default: break;
			}

			return {};
		}

		// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L861
		[[nodiscard]]
		static SoftUnit LoadBlock(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& state = *ctx.state;
			const uint8 tileIndex = GetBits<24, 26>(cmd.Data<0>());
			auto&& descriptor = state.tiles[tileIndex];
			const uint16 sl = GetBits<44, 55>(cmd.Data<0>());
			const uint16 tl = GetBits<32, 43>(cmd.Data<0>());
			const uint16 sh = GetBits<12, 23>(cmd.Data<0>());
			const FixedPoint16<5, 11> dxt = GetBits<0, 11>(cmd.Data<0>());

			const uint32 tmemBase = descriptor.tmemAddr * sizeof(uint64);
			const uint32 dramBase = state.textureImage.dramAddr;
			const uint8 bytesPerTexel = GetBytesParTexel(state.textureImage.size);

			FixedPoint16<5, 11> t{};
			t.Int().Set(tl);
			int tmemXor{};
			switch (state.textureImage.size)
			{
			case TexelSize::Px16:
				for (uint32 i = 0; i <= sh - sl; i++)
				{
					const uint32 s = i + sl;
					// if ((i * bytesPerTexel) % 8 == 0)
					// {
					// 	t += dxt;
					// 	tmemXor = t.Int() & 1 ? 4 : 0;
					// }
					const uint32 dramTexelAddress = dramBase + s * bytesPerTexel;
					const uint16 texel = ReadRdram<uint16>(ctx, dramTexelAddress);

					uint16 tmemTexelAddress = (tmemBase + (s * bytesPerTexel));
					// tmemTexelAddress ^= tmemXor;
					tmemTexelAddress &= TmemSizeMask_0xFFF;
					WriteTmem<uint16>(state, tmemTexelAddress, texel);
				}
				break;
			default:
				throw NotImplementedError();
			}
			return {};
		}

		// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.cpp#L901
		[[nodiscard]]
		static SoftUnit LoadTile(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			switch (ctx.state->textureImage.size)
			{
			case TexelSize::Px4:
				break;
			case TexelSize::Px8:
				LoaderTile<TexelSize::Px8>(ctx, cmd);
				return {};
			case TexelSize::Px16:
				LoaderTile<TexelSize::Px16>(ctx, cmd);
				return {};
			case TexelSize::Px32:
				LoaderTile<TexelSize::Px32>(ctx, cmd);
				return {};
			default: ;
			}
			throw NotImplementedError();
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
			otherModes.cycleType = static_cast<CycleType>(GetBits<52, 53>(cmd.Data<0>()));
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
			tile.size = static_cast<TexelSize>(GetBits<51, 52>(cmd.Data<0>()));
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
