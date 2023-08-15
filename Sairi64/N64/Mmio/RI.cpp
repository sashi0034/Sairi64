#include "stdafx.h"
#include "RI.h"

#include "N64/N64Logger.h"

namespace N64::Mmio
{
	RI::RI() :
		m_mode{0xE},
		m_config{040},
		m_select{0x14},
		m_refresh{0x63634}
	{
	}

	uint32 RI::Read32(PAddr32 paddr) const
	{
		switch (paddr)
		{
		case RiAddress::Mode_0x04700000:
			return m_mode;
		case RiAddress::Config_0x04700004:
			return m_config;
		case RiAddress::CurrentLoad_0x04700008:
			// https://n64brew.dev/wiki/RDRAM_Interface#0x0470_0004_-_RI_CONFIG
			return 0; // ?
		case RiAddress::Select_0x0470000C:
			return m_select;
		case RiAddress::Refresh_0x04700010:
			return m_refresh;
		default: break;
		}

		N64Logger::Abort();
		return 0;
	}

	void RI::Write32(PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case RiAddress::Mode_0x04700000:
			m_mode = value;
			return;
		case RiAddress::Config_0x04700004:
			m_config = value;
			return;
		case RiAddress::CurrentLoad_0x04700008:
			// https://n64brew.dev/wiki/RDRAM_Interface#0x0470_0004_-_RI_CONFIG
			return;
		case RiAddress::Select_0x0470000C:
			m_select = value;
			return;
		case RiAddress::Refresh_0x04700010:
			m_refresh = value;
			return;
		default: break;
		}

		N64Logger::Abort();
	}
}
