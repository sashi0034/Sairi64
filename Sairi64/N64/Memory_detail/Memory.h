#pragma once
#include "Rom.h"

namespace N64
{
	class N64System;

	constexpr uint32 RdramSize_0x00800000 = 0x00800000;
	constexpr uint32 RdramSizeMask_0x007FFFFF = RdramSize_0x00800000 - 1;
}

namespace N64::Memory_detail
{
	class Memory
	{
	public:
		Memory();
		Rom& GetRom() { return m_rom; }
		bool IsRomPal() const { return m_rom.IsPal(); }
		std::array<uint8, RdramSize_0x00800000>& Rdram() { return m_rdram; }

	private:
		Rom m_rom{};
		std::array<uint8, RdramSize_0x00800000> m_rdram{};
	};
}
