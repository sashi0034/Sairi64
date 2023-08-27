#pragma once

#include "N64/Forward.h"
#include "Controller.h"

namespace N64::Mmio
{
	constexpr uint32 PifRamSize_0x40 = 0x40;

	using PifRam = std::array<uint8, PifRamSize_0x40>;

	class Pif
	{
	public:
		PifRam& Ram() { return m_ram; }
		Controller& GetController() { return m_controller; }
		void ProcessCommands();

		static void ExecuteRom(N64System& n64);

	private:
		class Impl;

		PifRam m_ram{};

		Controller m_controller{};
	};
}
