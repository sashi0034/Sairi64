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
		std::array<uint8, RdramSize_0x00800000>& Rdram() { return m_rdram; }
		Array<uint8>& Sram() { return m_sram; }

		const Rom& GetRom() const { return m_rom; }
		const std::array<uint8, RdramSize_0x00800000>& Rdram() const { return m_rdram; }
		const Array<uint8>& Sram() const { return m_sram; }

		bool IsRomPal() const { return m_rom.IsPal(); }

	private:
		Rom m_rom{};
		std::array<uint8, RdramSize_0x00800000> m_rdram{};
		Array<uint8> m_sram{};
	};
}
