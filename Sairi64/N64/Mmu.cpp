#include "stdafx.h"
#include "Mmu.h"

#include "N64Logger.h"

namespace N64::Mmu
{
	template <class...>
	constexpr std::false_type always_false{};

	inline Optional<PAddr32> resolveVAddrInternal(Cpu& cpu, uint64 vaddr)
	{
		// TODO: 64 bit mode

		if (VMap::KSEG0.IsBetween(vaddr))
		{
			return PAddr32(VMap::KSEG0.Offset(vaddr));
		}
		if (VMap::KSEG1.IsBetween(vaddr))
		{
			return PAddr32(VMap::KSEG1.Offset(vaddr));
		}

		N64Logger::Abort();
		return none;
	}

	Optional<PAddr32> ResolveVAddr(Cpu& cpu, uint64 vaddr)
	{
		const Optional<PAddr32> paddr = resolveVAddrInternal(cpu, vaddr);
		N64_TRACE(paddr.has_value()
			? U"address translation vaddr:{:#016x} => paddr:{:#08x}"_fmt(vaddr, static_cast<uint32>(paddr.value()))
			: U"address translation vaddr:{:#016x} => failed"_fmt(vaddr));
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

		if (PMap::RdramMemory.IsBetween(paddr))
		{
			return PMap::RdramMemory.Offset(paddr);
			return 0;
		}
	}

	uint64 ReadPaddr64(N64System& n64, PAddr32 paddr)
	{
		return readPaddr<uint64>(n64, paddr);
	}
}
