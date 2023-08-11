#pragma once
#include "Rom.h"

namespace N64
{
	class N64System;

	constexpr uint32 RdramSize_0x00800000 = 0x00800000;
}

namespace N64::Memory_detail
{
	class Memory
	{
	public:
		Memory();
		Rom& GetRom() { return m_rom; }
		Array<uint8>& Rdram() { return m_rdram; }

	private:
		Rom m_rom{};
		Array<uint8> m_rdram{};
	};
}
