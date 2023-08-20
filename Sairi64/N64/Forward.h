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
