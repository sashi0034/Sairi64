#pragma once
#include "Utils/Util.h"

namespace N64
{
	constexpr uint32 CpuFreq_93750000 = 93750000;

	// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/common.hpp#L22
	inline uint32 GetCpuCyclesPerFrame_1562500(bool pal)
	{
		return pal ? CpuFreq_93750000 / 50 : CpuFreq_93750000 / 60; // 1875000 or 1562500
	}

	inline uint32 GetVideoFreq_48681812(bool pal)
	{
		return pal ? 4965'6530 : 4868'1812;
	}

	template <typename Data, typename Addr> inline Addr EndianAddress(Addr address)
	{
		static_assert(std::is_convertible<Addr, uint32>::value || std::is_convertible<Addr, uint64>::value);
		if constexpr (std::is_same<Data, uint8>::value)
			return address ^ 0b11;
		else if constexpr (std::is_same<Data, uint16>::value)
			return address ^ 0b10;
		else if constexpr (std::is_same<Data, uint32>::value || std::is_same<Data, uint64>::value)
			return address;
		else
			static_assert(Utils::AlwaysFalse<Data>);
	}

	class PAddr32 : public Utils::EnumValue<uint32>
	{
	public:
		constexpr explicit PAddr32(uint32 addr): EnumValue(addr) { return; }
	};

	enum class BusAccess
	{
		Load,
		Store,
	};

	namespace Cpu_detail
	{
		class Cpu;
	}

	using Cpu = Cpu_detail::Cpu;

	namespace Memory_detail
	{
		class Memory;
	}

	using Memory = Memory_detail::Memory;

	class N64System;
}
