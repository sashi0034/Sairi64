﻿#pragma once
#include "Cpu.h"
#include "Utils/Util.h"

namespace N64
{
	using namespace Utils;

	class PAddr32 : public EnumValue<uint32>
	{
	public:
		constexpr explicit PAddr32(unsigned v): EnumValue(v) { return; }
	};

	namespace Mmu
	{
		// https://n64brew.dev/wiki/Memory_map

		namespace VMap
		{
			constexpr AddressRange<uint32> KUSEG{0x00000000, 0x7FFFFFFF};
			constexpr AddressRange<uint32> KSEG0{0x80000000, 0x9FFFFFFF};
			constexpr AddressRange<uint32> KSEG1{0xA0000000, 0xBFFFFFFF};
			constexpr AddressRange<uint32> KSSEG{0xC0000000, 0xDFFFFFFF};
			constexpr AddressRange<uint32> KSEG3{0xE0000000, 0xFFFFFFFF};
		}

		Optional<PAddr32> ResolveVAddr(Cpu& cpu, uint64 vaddr);

		namespace PMap
		{
			constexpr AddressRange<uint32> RdramMemory{0x00000000, 0x007FFFFF};
			constexpr AddressRange<uint32> RspDmem{0x04000000, 0x04000FFF};
			constexpr AddressRange<uint32> RspImem{0x04001000, 0x04001FFF};
			constexpr AddressRange<uint32> RspRegisters{0x04040000, 0x040BFFFF};
		}

		uint64 ReadPaddr64(N64System& n64, PAddr32 paddr);
		uint32 ReadPaddr32(N64System& n64, PAddr32 paddr);
		uint16 ReadPaddr16(N64System& n64, PAddr32 paddr);
		uint8 ReadPaddr8(N64System& n64, PAddr32 paddr);

		void WritePaddr64(N64System& n64, PAddr32 paddr, uint64 value);
		void WritePaddr32(N64System& n64, PAddr32 paddr, uint32 value);
		void WritePaddr16(N64System& n64, PAddr32 paddr, uint16 value);
		void WritePaddr8(N64System& n64, PAddr32 paddr, uint8 value);
	}
}