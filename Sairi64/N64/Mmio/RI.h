#pragma once
#include "N64/Types.h"

namespace N64::Mmio
{
	namespace RiAddress
	{
		constexpr PAddr32 Mode_0x04700000{0x04700000};
		constexpr PAddr32 Config_0x04700004{0x04700004};
		constexpr PAddr32 CurrentLoad_0x04700008{0x04700008};
		constexpr PAddr32 Select_0x0470000C{0x0470000C};
		constexpr PAddr32 Refresh_0x04700010{0x04700010};
	}

	// https://n64brew.dev/wiki/RDRAM_Interface
	// RDRAM Interface
	class RI
	{
	public:
		RI();
		uint32 Read32(PAddr32 paddr) const;
		void Write32(PAddr32 paddr, uint32 value);

	private:
		uint32 m_mode{};
		uint32 m_config{};
		uint32 m_select{};
		uint32 m_refresh{};
	};
}
