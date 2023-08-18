#include "stdafx.h"
#include "PI.h"

#include "N64/N64Logger.h"

namespace N64::Mmio
{
	uint32 PI::Read32(N64System& n64, PAddr32 paddr) const
	{
		switch (paddr)
		{
		case PiAddress::DramAddr_0x04600000:
			return m_dramAddr;
		case PiAddress::CartAddr_0x04600004:
			return m_cartAddr;
		case PiAddress::RdLen_0x04600008:
			return m_rdLen;
		case PiAddress::WrLen_0x0460000C:
			return m_wrLen;
		case PiAddress::Status_0x04600010:
			return m_status;
		case PiAddress::BsdDom1Lat_0x04600014:
			return m_bsdDom1Lat;
		case PiAddress::BsdDom1Pwd_0x04600018:
			return m_bsdDom1Pwd;
		case PiAddress::BsdDom1Pgs_0x0460001C:
			return m_bsdDom1Pgs;
		case PiAddress::BsdDom1Rls_0x04600020:
			return m_bsdDom1Rls;
		case PiAddress::BsdDom2Lat_0x04600024:
			return m_bsdDom2Lat;
		case PiAddress::BsdDom2Pwd_0x04600028:
			return m_bsdDom2Pwd;
		case PiAddress::BsdDom2Pgs_0x0460002C:
			return m_bsdDom2Pgs;
		case PiAddress::BsdDom2Rls_0x04600030:
			return m_bsdDom2Rls;
		default: break;
		}

		N64Logger::Abort();
		return 0;
	}

	void PI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case PiAddress::DramAddr_0x04600000:
			m_dramAddr = value & 0x00FF'FFFF;
			return;
		case PiAddress::CartAddr_0x04600004:
			m_cartAddr = value;
			return;
		case PiAddress::RdLen_0x04600008:
			// TODO: DMA Read
			break;
		case PiAddress::WrLen_0x0460000C:
			// TODO: DMA Write
			break;
		case PiAddress::Status_0x04600010:
			// TODO
			break;
		case PiAddress::BsdDom1Lat_0x04600014:
			m_bsdDom1Lat = value & 0xFF;
			return;
		case PiAddress::BsdDom1Pwd_0x04600018:
			m_bsdDom1Pwd = value & 0xFF;
			return;
		case PiAddress::BsdDom1Pgs_0x0460001C:
			m_bsdDom1Pgs = value & 0xFF;
			return;
		case PiAddress::BsdDom1Rls_0x04600020:
			m_bsdDom1Rls = value & 0xFF;
			return;
		case PiAddress::BsdDom2Lat_0x04600024:
			m_bsdDom2Lat = value & 0xFF;
			return;
		case PiAddress::BsdDom2Pwd_0x04600028:
			m_bsdDom2Pwd = value & 0xFF;
			return;
		case PiAddress::BsdDom2Pgs_0x0460002C:
			m_bsdDom2Pgs = value & 0xFF;
			return;
		case PiAddress::BsdDom2Rls_0x04600030:
			m_bsdDom2Rls = value & 0xFF;
			return;
		default: break;
		}

		N64Logger::Abort();
	}
}
