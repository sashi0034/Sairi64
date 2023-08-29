#pragma once
#include "Utils/Util.h"

// デバッグ無しのリリースビルド設定
#define N64_RELEASE_ENABLE false

namespace N64
{
	constexpr uint32 CpuFreq_93750000 = 93750000;

	inline uint32 GetFps_60_50(bool pal)
	{
		return pal ? 50 : 60;
	}

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
			return address ^ 0b10; // (address & (~0b1)) ^ 0b10
		else if constexpr (std::is_same<Data, uint32>::value || std::is_same<Data, uint64>::value)
			return address;
		else
		{
			static_assert(Utils::AlwaysFalse<Data>);
			return {};
		}
	}

	class PAddr32 : public Utils::EnumValue<uint32>
	{
	public:
		constexpr explicit PAddr32(uint32 addr): EnumValue(addr) { return; }
	};

	using CpuCycles = int32;

	enum class ProcessorType
	{
		Interpreter,
		Dynarec,
	};

	enum class BusAccess
	{
		Load,
		Store,
	};

	enum class ViType4
	{
		Blank = 0,
		Reserved = 1,
		// 16-bit
		R5G5B5A1 = 2,
		// 32-bit
		R8G8B8A8 = 3,
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
