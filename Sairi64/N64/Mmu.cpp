#include "stdafx.h"
#include "Mmu.h"

#include "N64Logger.h"

namespace N64::Mmu
{
	inline Optional<uint32> resolveVAddrInternal(Cpu& cpu, uint64 vaddr)
	{
		// TODO: 64 bit mode

		if (Address::KSEG0.IsBetween(vaddr))
		{
			return vaddr - Address::KSEG0.base;
		}
		if (Address::KSEG1.IsBetween(vaddr))
		{
			return vaddr - Address::KSEG1.base;
		}

		N64Logger::Abort();
		return none;
	}

	Optional<uint32>ResolveVAddr (Cpu& cpu, uint64 vaddr)
	{
		const Optional<uint32> paddr = resolveVAddrInternal(cpu, vaddr);
		N64_TRACE(paddr.has_value()
			? U"address translation vaddr:{:#016x} => paddr:{:#08x}"_fmt(vaddr, paddr.value())
			: U"address translation vaddr:{:#016x} => failed"_fmt(vaddr));
		return paddr;
	}
}
