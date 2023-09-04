#pragma once
#include "Utils/Util.h"

namespace N64::Rsp_detail
{
	inline uint8 VuElementIndex(uint8 index) { return 7 - index; };
	inline uint8 VuByteIndex(uint8 index) { return 15 - index; };

	template <typename T>
	inline uint16 VuFlag16(T flag) { return flag ? 0xFFFF : 0; };

	union Vpr_t
	{
		std::array<sint16, 8> signedElements;
		std::array<uint16, 8> elements;
		std::array<uint8, 16> bytes;
		uint128 single;
	};

	static_assert(sizeof(Vpr_t) == 16);

	struct VprDouble
	{
		Vpr_t l;
		Vpr_t h;
	};

	struct VprTriple
	{
		Vpr_t l;
		Vpr_t m;
		Vpr_t h;
	};

	// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core#Vector_Unit_(VU)
	// Vector Unit
	struct VU
	{
		std::array<Vpr_t, 32> reg;
		VprDouble vcO;
		VprDouble vcC;
		Vpr_t vcE;
		VprTriple accum;
	};

	enum class VuControl
	{
		VcO,
		VcC,
		VcE
	};
}
