#include "stdafx.h"
#include "Memory.h"

namespace N64::Memory_detail
{
	Memory::Memory()
	{
		m_rdram.resize(RdramSize_0x00800000);
	}
}
