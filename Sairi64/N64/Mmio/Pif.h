﻿#pragma once

#include "N64/Forward.h"
#include "N64/Joybus/JoybusDevice.h"

namespace N64::Mmio
{
	constexpr uint32 PifRamSize_0x40 = 0x40;

	using PifRam = std::array<uint8, PifRamSize_0x40>;

	class Pif
	{
	public:
		Pif();

		PifRam& Ram() { return m_ram; }
		const PifRam& Ram() const { return m_ram; }

		void ProcessCommands();

		static void ExecuteRom(N64System& n64);

	private:
		class Impl;

		PifRam m_ram{};
		Joybus::JoybusDeviceManager m_deviceManager{};

		Array<uint8> m_controllerPack{};
	};
}
