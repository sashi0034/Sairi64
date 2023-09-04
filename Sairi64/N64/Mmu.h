#pragma once
#include "Cpu.h"
#include "Forward.h"
#include "Utils/Util.h"

namespace N64
{
	using namespace Utils;

	class ResolvedPAddr32
	{
	public:
		explicit ResolvedPAddr32(uint32 paddr) : m_paddr(paddr) { return; };

		static constexpr PAddr32 InvalidAddress = PAddr32(0xFFFF'FFFF);
		bool has_value() const { return m_paddr != InvalidAddress; }
		explicit operator bool() const { return has_value(); }
		PAddr32 value() const { return m_paddr; }

	private:
		PAddr32 m_paddr;
	};

	static_assert(sizeof(ResolvedPAddr32) == 4);

	namespace Mmu
	{
		// https://n64brew.dev/wiki/Memory_map

		namespace VMap
		{
			constexpr AddressRange<uint32> KUSEG{0x00000000, 0x7FFFFFFF};
			constexpr AddressRange<uint32> KSEG0{0x80000000, 0x9FFFFFFF};
			constexpr uint8 KSEG0_sr29_4 = 4; // KSEG0 >> 29
			constexpr AddressRange<uint32> KSEG1{0xA0000000, 0xBFFFFFFF};
			constexpr uint8 KSEG1_sr29_5 = 5; // KSEG1 >> 29
			constexpr AddressRange<uint32> KSSEG{0xC0000000, 0xDFFFFFFF};
			constexpr AddressRange<uint32> KSEG3{0xE0000000, 0xFFFFFFFF};
		}

		[[nodiscard]]
		N64_ABI ResolvedPAddr32 ResolveVAddr(Cpu& cpu, uint64 vaddr);

		namespace PMap
		{
			constexpr AddressRange<uint32> RdramMemory{0x00000000, 0x007FFFFF};
			constexpr AddressRange<uint32> SpDmem{0x04000000, 0x04000FFF};
			constexpr AddressRange<uint32> SpImem{0x04001000, 0x04001FFF};
			constexpr AddressRange<uint32> RspRegisters{0x04040000, 0x040BFFFF};

			constexpr AddressRange<uint32> PifRam{0x1FC007C0, 0x1FC007FF};
			constexpr AddressRange<uint32> MI{0x04300000, 0x043FFFFF};
			constexpr AddressRange<uint32> VI{0x04400000, 0x044FFFFF};
			constexpr AddressRange<uint32> AI{0x04500000, 0x045FFFFF};
			constexpr AddressRange<uint32> PI{0x04600000, 0x046FFFFF};
			constexpr AddressRange<uint32> RI{0x04700000, 0x047FFFFF};
			constexpr AddressRange<uint32> SI{0x04800000, 0x048FFFFF};

			constexpr AddressRange<uint32> Rom{0x10000000, 0x1FBFFFFF};
		}

		N64_ABI uint64 ReadPaddr64(N64System& n64, PAddr32 paddr);
		N64_ABI uint32 ReadPaddr32(N64System& n64, PAddr32 paddr);
		N64_ABI uint16 ReadPaddr16(N64System& n64, PAddr32 paddr);
		N64_ABI uint8 ReadPaddr8(N64System& n64, PAddr32 paddr);

		N64_ABI void WritePaddr64(N64System& n64, PAddr32 paddr, uint64 value64);
		N64_ABI void WritePaddr32(N64System& n64, PAddr32 paddr, uint32 value32);
		N64_ABI void WritePaddr16(N64System& n64, PAddr32 paddr, uint32 value32);
		N64_ABI void WritePaddr8(N64System& n64, PAddr32 paddr, uint32 value32);
	}
}
