﻿#pragma once

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

	class Color32Bpp
	{
	public:
		Color32Bpp(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto A() { return BitAccess<0, 7>(m_raw); }
		auto B() { return BitAccess<8, 15>(m_raw); }
		auto G() { return BitAccess<16, 23>(m_raw); }
		auto R() { return BitAccess<24, 31>(m_raw); }

	private:
		uint32 m_raw{};
	};

	class Color16Bpp
	{
	public:
		Color16Bpp(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto A() { return BitAccess<0>(m_raw); }
		auto B() { return BitAccess<1, 5>(m_raw); }
		auto G() { return BitAccess<6, 10>(m_raw); }
		auto R() { return BitAccess<11, 15>(m_raw); }

	private:
		uint32 m_raw{};
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

		Color32Bpp blendColor;

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
		std::span<uint8> rdram;
	};
}