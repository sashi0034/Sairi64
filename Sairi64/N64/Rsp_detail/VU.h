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
		std::array<sint16, 8> sE;
		std::array<uint16, 8> uE;
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
		std::array<Vpr_t, 32> regs;
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

	struct DivIO
	{
		sint16 divIn;
		sint16 divOut;
		bool divInLoaded;
	};

	[[nodiscard]]
	inline Vpr_t BroadcastVt(const Vpr_t& vt, int l0, int l1, int l2, int l3, int l4, int l5, int l6, int l7)
	{
		Vpr_t vte{};
		vte.uE[VuElementIndex(0)] = vt.uE[VuElementIndex(l0)];
		vte.uE[VuElementIndex(1)] = vt.uE[VuElementIndex(l1)];
		vte.uE[VuElementIndex(2)] = vt.uE[VuElementIndex(l2)];
		vte.uE[VuElementIndex(3)] = vt.uE[VuElementIndex(l3)];
		vte.uE[VuElementIndex(4)] = vt.uE[VuElementIndex(l4)];
		vte.uE[VuElementIndex(5)] = vt.uE[VuElementIndex(l5)];
		vte.uE[VuElementIndex(6)] = vt.uE[VuElementIndex(l6)];
		vte.uE[VuElementIndex(7)] = vt.uE[VuElementIndex(l7)];
		return vte;
	}

	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/rsp/instructions.cpp#L117
	[[nodiscard]]
	inline Vpr_t GetVtE(const Vpr_t& vt, uint8 e)
	{
		switch (e & 0xF)
		{
		case 0: [[fallthrough]];
		case 1:
			return vt;
		case 2: [[fallthrough]];
		case 3:
			return BroadcastVt(vt, e - 2, e - 2, e, e, e + 2, e + 2, e + 4, e + 4);
		case 4: [[fallthrough]];
		case 5: [[fallthrough]];
		case 6: [[fallthrough]];
		case 7: [[fallthrough]];
			return BroadcastVt(vt, e - 4, e - 4, e - 4, e - 4, e, e, e, e);
		default: // [8, 15]
		{
			Vpr_t vte{};
			const int index = VuElementIndex(e - 8);
			for (int i = 0; i < 8; ++i)
			{
				vte.uE[i] = vt.uE[index];
			}
			return vte;
		}
		}
	}
}
