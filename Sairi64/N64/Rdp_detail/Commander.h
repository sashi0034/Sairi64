#pragma once

#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.h#L26
	enum class BlendSource
	{
		// colors
		PixelColor,
		MemoryColor,
		BlendColor,
		FogColor,

		// Alphas
		PixelAlpha,
		PrimitiveAlpha,
		ShadeAlpha,
		OneMinusAlpha,
		MemoryAlpha,
		One,
		Zero,
	};

	struct BlendConfig
	{
		BlendSource source1a;
		BlendSource source1b;
		BlendSource source2a;
		BlendSource source2b;
	};

	// https://github.com/Dillonb/n64/blob/91c198fe60c8a4e4c4e9e12b43f24157f5e21347/src/rdp/softrdp.h#L70
	struct CommanderState
	{
		struct
		{
			uint16 xl;
			uint16 yl;
			uint16 xh;
			uint16 yh;
			bool f;
			bool o;
		} scissorRect;

		struct
		{
			uint8 format;
			uint8 size;
			uint16 width;
			uint32 dramAddr;
		} colorImage;

		struct
		{
			bool atomicPrim;
			uint8 cycleType;
			bool perspTexEn;
			bool detailTexEn;
			bool sharpenTexEn;
			bool texLodEn;
			bool enTlut;
			bool tlutType;
			bool sampleType;
			bool midTexel;
			bool biLerp0;
			bool biLerp1;
			bool convertOne;
			bool keyEn;
			uint8 rgbDitherSel;
			uint8 alphaDitherSel;

			BlendConfig blenderConfig[2]; // one config for each cycle

			bool forceBlend;
			bool alphaCvgSelect;
			bool cvgTimesAlpha;

			uint8 zMode;
			uint8 cvgDest;

			bool colorOnCvg;
			bool imageReadEn;
			bool zUpdateEn;
			bool zCompareEn;
			bool antialiasEn;
			bool zSourceSel;
			bool ditherAlphaEn;
			bool alphaCompareEn;
		} otherModes;
	};

	struct CommanderContext
	{
		CommanderState* state;
	};
}
