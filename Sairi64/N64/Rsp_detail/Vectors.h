#pragma once
#include "SpRom.h"
#include "Utils/Util.h"

namespace N64::Rsp_detail
{
	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L141
	inline sint32 SignExtend7bitOffset(uint8 offset, uint8 shiftAmount)
	{
		const sint8 signedOffset = ((offset << 1) & 0x80) | offset;
		const sint32 offset1 = (sint32)signedOffset;
		const uint32 offset2 = offset1;
		return static_cast<sint32>(offset2 << shiftAmount);
	}

	inline uint32 Clz32(uint32 value)
	{
		// if (value == 0)
		// {
		// 	return 32;
		// }
		unsigned long index;
		_BitScanReverse(&index, value);

		return 31 - index;
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L148
	inline uint32 Rcp(sint32 input1)
	{
		const sint32 mask = input1 >> 31;
		sint32 input2 = input1 ^ mask;
		if (input1 > INT16_MIN)
		{
			input2 -= mask;
		}
		if (input2 == 0)
		{
			return 0x7FFFFFFF;
		}
		else if (input1 == INT16_MIN)
		{
			return 0xFFFF0000;
		}

		const uint32 shift = Clz32(input2);
		const uint64 input64 = static_cast<uint64>(input2);
		const uint32 index = ((input64 << shift) & 0x7FC00000) >> 22;

		static constexpr auto rcpRom = RcpRom;
		sint32 result = rcpRom[index];
		result = (0x10000 | result) << 14;
		result = (result >> (31 - shift)) ^ mask;
		return result;
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L171
	inline uint32 Rsq(uint32 input)
	{
		if (input == 0)
		{
			return 0x7FFFFFFF;
		}
		else if (input == 0xFFFF8000)
		{
			return 0xFFFF0000;
		}
		else if (input > 0xFFFF8000)
		{
			input--;
		}

		const sint32 mask = static_cast<sint32>(input) >> 31;
		input ^= mask;

		const uint32 shift = Clz32(input) + 1;

		const int index = (((input << shift) >> 24) | ((shift & 1) << 8));
		const uint32 rom = (static_cast<uint32>(RsqRom[index]) << 14);
		const int rShift = ((32 - shift) >> 1);
		const uint32 result = (0x40000000 | rom) >> rShift;

		return result ^ mask;
	}

	// https://github.com/SimoneN64/Kaizen/blob/56ab73865271635d887eab96a0e51873347abe77/src/backend/core/rsp/instructions.cpp#L705
	inline sint16 SignedClamp(sint64 value)
	{
		if (value < -32768) return -32768;
		if (value > 32767) return 32767;
		return static_cast<int16_t>(value);
	}

	inline uint16 UnsignedClamp(sint64 value)
	{
		if (value < 0) return 0;
		if (value > 32767) return 65535;
		return value;
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L31
	inline bool IsSignExtension(sint16 high, sint16 low)
	{
		if (high == 0)
		{
			return (low & 0x8000) == 0;
		}
		else if (high == -1)
		{
			return (low & 0x8000) == 0x8000;
		}
		return false;
	}
}
