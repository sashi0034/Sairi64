#include "stdafx.h"
#include "Memory.h"

namespace N64::Memory_detail
{
	constexpr uint32 sramSize_256KiB = 256 * 1024; // 32768

	Memory::Memory()
	{
		m_sram.resize(sramSize_256KiB);
	}
}
