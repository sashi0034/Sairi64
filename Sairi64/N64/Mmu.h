﻿#pragma once
#include "Cpu.h"
#include "Forward.h"
#include "Utils/Util.h"

namespace N64
{
	using namespace Utils;

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

		[[nodiscard]]
		Optional<PAddr32> ResolveVAddr(Cpu& cpu, uint64 vaddr);

		namespace PMap
		{
			constexpr AddressRange<uint32> RdramMemory{0x00000000, 0x007FFFFF};
			constexpr AddressRange<uint32> SpDmem{0x04000000, 0x04000FFF};
			constexpr AddressRange<uint32> SpImem{0x04001000, 0x04001FFF};
			constexpr AddressRange<uint32> RspRegisters{0x04040000, 0x040BFFFF};

			constexpr AddressRange<uint32> PifRam{0x1FC007C0, 0x1FC007FF};
			constexpr AddressRange<uint32> MI{0x04300000, 0x043FFFFF};
			constexpr AddressRange<uint32> AI{0x04500000, 0x045FFFFF};
			constexpr AddressRange<uint32> PI{0x04600000, 0x046FFFFF};
			constexpr AddressRange<uint32> RI{0x04700000, 0x047FFFFF};
			constexpr AddressRange<uint32> SI{0x04800000, 0x048FFFFF};

			constexpr AddressRange<uint32> Rom{0x10000000, 0x1FBFFFFF};
		}

		uint64 ReadPaddr64(N64System& n64, PAddr32 paddr);
		uint32 ReadPaddr32(N64System& n64, PAddr32 paddr);
		uint16 ReadPaddr16(N64System& n64, PAddr32 paddr);
		uint8 ReadPaddr8(N64System& n64, PAddr32 paddr);

		void WritePaddr64(N64System& n64, PAddr32 paddr, uint64 value64);
		void WritePaddr32(N64System& n64, PAddr32 paddr, uint32 value32);
		void WritePaddr16(N64System& n64, PAddr32 paddr, uint32 value32);
		void WritePaddr8(N64System& n64, PAddr32 paddr, uint32 value32);
	}
}
