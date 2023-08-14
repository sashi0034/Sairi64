#include "stdafx.h"
#include "Mmu.h"

#include "N64Logger.h"
#include "N64System.h"

namespace N64::Mmu
{
	template <class...>
	constexpr std::false_type always_false{};

	inline Optional<PAddr32> resolveVAddrInternal(Cpu& cpu, uint64 vaddr)
	{
		// TODO: 64 bit mode

		if (VMap::KSEG0.IsBetween(vaddr)) // 0x80000000, 0x9FFFFFFF
		{
			return PAddr32(vaddr - VMap::KSEG0.base);
		}
		if (VMap::KSEG1.IsBetween(vaddr)) // 0xA0000000, 0xBFFFFFFF
		{
			return PAddr32(vaddr - VMap::KSEG1.base);
		}

		N64Logger::Abort(U"unsupported vaddr: {:016x}"_fmt(vaddr));
		return none;
	}

	Optional<PAddr32> ResolveVAddr(Cpu& cpu, uint64 vaddr)
	{
		const Optional<PAddr32> paddr = resolveVAddrInternal(cpu, vaddr);
		N64_TRACE(paddr.has_value()
			? U"address translation vaddr:{:#018x} => paddr:{:#010x}"_fmt(vaddr, static_cast<uint32>(paddr.value()))
			: U"address translation vaddr:{:#018x} => failed"_fmt(vaddr));
		return paddr;
	}

	template <typename Wire>
	Wire readPaddr(N64System& n64, PAddr32 paddr)
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		constexpr bool wire16 = std::is_same<Wire, uint16_t>::value;
		constexpr bool wire8 = std::is_same<Wire, uint8_t>::value;
		static_assert(wire64 || wire32 || wire16 || wire8);

		if (PMap::RdramMemory.IsBetween(paddr)) // 0x00000000, 0x007FFFFF
		{
			const uint32 offset = paddr - PMap::RdramMemory.base;
			return ReadBytes<Wire>(n64.GetMemory().Rdram(), offset);
		}
		else if (PMap::SpDmem.IsBetween(paddr)) // 0x04000000, 0x04000FFF
		{
			const uint32 offset = paddr - PMap::SpDmem.base;
			return n64.GetRsp().ReadDmem<Wire>(offset);
		}
		else if (PMap::SpImem.IsBetween(paddr)) // 0x04001000, 0x04001FFF
		{
			const uint32 offset = paddr - PMap::SpImem.base;
			return n64.GetRsp().ReadImem<Wire>(offset);
		}
		else if (PMap::PifRam.IsBetween(paddr)) // 0x1FC007C0, 0x1FC007FF
		{
			if constexpr (wire32)
			{
				const uint64 offset = paddr - PMap::PifRam.base;
				return ReadBytes32(n64.GetSI().GetPifRam(), offset);
			}
		}

		N64Logger::Abort(U"read unsupported paddr {}-bit: {:08X}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), static_cast<uint32>(paddr)));
		return 0;
	}

	template <typename Wire>
	void writePaddr(N64System& n64, PAddr32 paddr, Wire value)
	{
		constexpr bool wire64 = std::is_same<Wire, uint64_t>::value;
		constexpr bool wire32 = std::is_same<Wire, uint32_t>::value;
		constexpr bool wire16 = std::is_same<Wire, uint16_t>::value;
		constexpr bool wire8 = std::is_same<Wire, uint8_t>::value;
		static_assert(wire64 || wire32 || wire16 || wire8);

		if (PMap::RdramMemory.IsBetween(paddr)) // 0x00000000, 0x007FFFFF
		{
			const uint32 offset = paddr - PMap::RdramMemory.base;
			return WriteBytes<Wire>(n64.GetMemory().Rdram(), offset, value);
		}
		else if (PMap::SpDmem.IsBetween(paddr)) // 0x04000000, 0x04000FFF
		{
			const uint32 offset = paddr - PMap::SpDmem.base;
			return n64.GetRsp().WriteDmem<Wire>(offset, value);
		}
		else if (PMap::SpImem.IsBetween(paddr)) // 0x04001000, 0x04001FFF
		{
			const uint32 offset = paddr - PMap::SpImem.base;
			return n64.GetRsp().WriteImem<Wire>(offset, value);
		}
		else if (PMap::PifRam.IsBetween(paddr)) // 0x1FC007C0, 0x1FC007FF
		{
			if constexpr (wire32)
			{
				const uint64 offset = paddr - PMap::PifRam.base;
				return WriteBytes32(n64.GetSI().GetPifRam(), offset, value);
			}
		}

		N64Logger::Abort(U"write unsupported paddr {}-bit: {:08X}"_fmt(
			static_cast<int>(std::numeric_limits<Wire>::digits), static_cast<uint32>(paddr)));
	}

	uint64 ReadPaddr64(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint64>(n64, paddr);
	}

	uint32 ReadPaddr32(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint32>(n64, paddr);
	}

	uint16 ReadPaddr16(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint16>(n64, paddr);
	}

	uint8 ReadPaddr8(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint8>(n64, paddr);
	}

	void WritePaddr64(N64System& n64, PAddr32 paddr, uint64 value)
	{
		writePaddr<uint64>(n64, paddr, value);
	}

	void WritePaddr32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		writePaddr<uint32>(n64, paddr, value);
	}

	void WritePaddr16(N64System& n64, PAddr32 paddr, uint16 value)
	{
		writePaddr<uint16>(n64, paddr, value);
	}

	void WritePaddr8(N64System& n64, PAddr32 paddr, uint8 value)
	{
		writePaddr<uint8>(n64, paddr, value);
	}
}
