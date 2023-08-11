#pragma once
#include "Cpu.h"
#include "Utils/Util.h"

namespace N64
{
	namespace Mmu
	{
		using namespace Utils;

		namespace Address
		{
			constexpr AddressRange<uint32> KUSEG{0x00000000, 0x7FFFFFFF};
			constexpr AddressRange<uint32> KSEG0{0x80000000, 0x9FFFFFFF};
			constexpr AddressRange<uint32> KSEG1{0xA0000000, 0xBFFFFFFF};
			constexpr AddressRange<uint32> KSSEG{0xC0000000, 0xDFFFFFFF};
			constexpr AddressRange<uint32> KSEG3{0xE0000000, 0xFFFFFFFF};
		}

		Optional<uint32> ResolveVAddr(Cpu& cpu, uint64 vaddr);
	}
}
