#pragma once

#include "../SoftCommander.h"

// https://ultra64.ca/files/documentation/silicon-graphics/SGI_RDP_Command_Summary.pdf

namespace N64::Rdp_detail
{
	class Soft
	{
	public:
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

			otherModes.blenderConfig[0].source1a = from1a(GetBits<30, 31>(cmd.Data<0>()));
			otherModes.blenderConfig[1].source1a = from1a(GetBits<28, 29>(cmd.Data<0>()));

			otherModes.blenderConfig[0].source1b = from1a(GetBits<26, 27>(cmd.Data<0>()));
			otherModes.blenderConfig[1].source1b = from1a(GetBits<24, 25>(cmd.Data<0>()));

			otherModes.blenderConfig[0].source2a = from1a(GetBits<22, 23>(cmd.Data<0>()));
			otherModes.blenderConfig[1].source2a = from1a(GetBits<20, 21>(cmd.Data<0>()));

			otherModes.blenderConfig[0].source2b = from1a(GetBits<18, 19>(cmd.Data<0>()));
			otherModes.blenderConfig[1].source2b = from1a(GetBits<24, 25>(cmd.Data<0>()));

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
		static SoftUnit SetColorImage(const CommanderContext& ctx, const RdpCommand& cmd)
		{
			auto&& colorImage = ctx.state->colorImage;
			colorImage.format = GetBits<53, 55>(cmd.Data<0>());
			colorImage.size = GetBits<51, 52>(cmd.Data<0>());
			colorImage.width = GetBits<32, 41>(cmd.Data<0>()) + 1;
			colorImage.dramAddr = GetBits<0, 25>(cmd.Data<0>());
			return {};
		}

	private:
		static BlendSource from1a(uint8 value)
		{
			switch (value)
			{
			case 0: return BlendSource::PixelAlpha;
			case 1: return BlendSource::MemoryColor;
			case 2: return BlendSource::BlendColor;
			case 3: return BlendSource::FogColor;
			default: return {};
			}
		}

		static BlendSource from1b(uint8 value)
		{
			switch (value)
			{
			case 0: return BlendSource::PixelAlpha;
			case 1: return BlendSource::PrimitiveAlpha;
			case 2: return BlendSource::ShadeAlpha;
			case 3: return BlendSource::Zero;
			default: return {};
			}
		}

		static BlendSource from2a(uint8 value)
		{
			switch (value)
			{
			case 0: return BlendSource::PixelAlpha;
			case 1: return BlendSource::MemoryColor;
			case 2: return BlendSource::BlendColor;
			case 3: return BlendSource::FogColor;
			default: return {};
			}
		}

		static BlendSource from2b(uint8 value)
		{
			switch (value)
			{
			case 0: return BlendSource::OneMinusAlpha;
			case 1: return BlendSource::MemoryAlpha;
			case 2: return BlendSource::One;
			case 3: return BlendSource::Zero;
			default: return {};
			}
		}
	};
}
