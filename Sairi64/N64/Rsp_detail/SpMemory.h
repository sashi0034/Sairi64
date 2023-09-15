#pragma once
#include "RspAddress.h"

namespace N64::Rsp_detail
{
	constexpr uint16 SpMemoryMask_0xFFF = 0xFFF;

	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/RSP.hpp#L8
	// アラインメントずれに注意
	template <uint32 memorySize>
	class SpMemory : public std::array<uint8, memorySize>
	{
	public:
		using addr_t = uint32;
		void WriteSpByte(addr_t addr, uint8 value) { spByte(addr) = value; }
		uint8 ReadSpByte(addr_t addr) const { return spByte(addr); }

		uint16 ReadSpHalf(addr_t addr) const
		{
			return (spByte(addr) << 8) | spByte(addr + 1);
		}

		void WriteSpHalf(addr_t addr, uint16 value)
		{
			spByte(addr) = (value >> 8) & 0xFF;
			spByte(addr + 1) = value & 0xFF;
		}

		uint32 ReadSpWord(addr_t addr) const
		{
			return (ReadSpHalf(addr) << 16) | ReadSpHalf(addr + 2);
		}

		void WriteSpWord(addr_t addr, uint32 value)
		{
			WriteSpHalf(addr, ((value) >> 16) & 0xFFFF);
			WriteSpHalf(addr + 2, value & 0xFFFF);
		}

		template <typename Wire>
		Wire ReadSpData(addr_t addr) const
		{
			if constexpr (std::same_as<Wire, uint8>) return ReadSpByte(addr);
			else if constexpr (std::same_as<Wire, uint16>) return ReadSpHalf(addr);
			else if constexpr (std::same_as<Wire, uint32>) return ReadSpWord(addr);
			else
			{
				static_assert(AlwaysFalse<Wire>);
				return {};
			}
		}

		template <typename Wire>
		void WriteSpData(addr_t addr, Wire value)
		{
			if constexpr (std::same_as<Wire, uint8>) WriteSpByte(addr, value);
			else if constexpr (std::same_as<Wire, uint16>) WriteSpHalf(addr, value);
			else if constexpr (std::same_as<Wire, uint32>) WriteSpWord(addr, value);
			else static_assert(AlwaysFalse<Wire>);
		}

	private:
		uint8& spByte(addr_t addr) { return (*this)[EndianAddress<uint8>(addr) & SpMemoryMask_0xFFF]; };
		uint8 spByte(addr_t addr) const { return (*this)[EndianAddress<uint8>(addr) & SpMemoryMask_0xFFF]; };
	};

	using SpDmem = SpMemory<SpDmemSize_0x1000>;
	using SpImem = SpMemory<SpDmemSize_0x1000>;
}
