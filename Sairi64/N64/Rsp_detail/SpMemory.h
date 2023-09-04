#pragma once
#include "RspAddress.h"

namespace N64::Rsp_detail
{
	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/RSP.hpp#L8
	// big endian
	template <uint32 memorySize>
	class SpMemory : public std::array<uint8, memorySize>
	{
	public:
		using addr_t = uint32;
		void SetSpByte(addr_t addr, uint8 value) { spByte(addr) = value; }
		uint8 GetSpByte(uint8 addr) const { return spByte(addr); }

		uint16 GetSpHalf(addr_t addr)
		{
			return (spByte(addr) << 8) | spByte(addr + 1);
		}

		void SetSpHalf(addr_t addr, uint16 value)
		{
			spByte(addr) = (value >> 8) & 0xFF;
			spByte(addr + 1) = value & 0xFF;
		}

		uint32 GetSpWord(addr_t addr)
		{
			return (GetSpHalf(addr) << 16) | GetSpHalf(addr + 2);
		}

		void SetSpWord(addr_t addr, uint32 value)
		{
			SetSpHalf(addr, ((value) >> 16) & 0xFFFF);
			SetSpHalf(addr + 2, value & 0xFFFF);
		}

	private:
		uint8& spByte(addr_t addr) { return *this[addr & SpImemMask_0xFFF]; };
		uint8 spByte(addr_t addr) const { return *this[addr & SpImemMask_0xFFF]; };
	};

	using SpDmem = SpMemory<SpDmemSize_0x1000>;
	using SpImem = SpMemory<SpDmemSize_0x1000>;
}
