#pragma once
#include "Rom.h"

namespace N64
{
	class N64System;
}

namespace N64::Memory_detail
{
	constexpr uint32 RdramSize_0x00800000 = 0x00800000;

	class Memory
	{
	public:
		Memory();
		Rom& GetRom() { return m_rom; }

	private:
		Rom m_rom{};
		Array<uint8> m_rdram{};
	};
}
