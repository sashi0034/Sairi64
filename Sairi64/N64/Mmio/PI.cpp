#include "stdafx.h"
#include "PI.h"

#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

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
		case PiAddress::Status_0x04600010: {
			PiStatus32 status = m_status;
			status.Interrupt().Set(n64.GetMI().GetInterrupt().Pi());
			return status;
		}
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

		N64Logger::Abort(U"unsupported pi read: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
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
			dmaWrite(n64, value);
			return;
		case PiAddress::Status_0x04600010:
			if (value & 2) InterruptLower<Interruption::PI>(n64);
			return;
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

		N64Logger::Abort(U"unsupported pi write: paddr={:08X}, value={:08X}"_fmt(static_cast<uint32>(paddr), value));
	}

	uint8 readCartridge(Memory& memory, uint32 address)
	{
		// TODO: 最適化
		if (Mmu::PMap::CartridgeRom.IsBetween(address))
			return memory.GetRom().Data()[EndianAddress<uint8>(address - Mmu::PMap::CartridgeRom.base)];
		else if (Mmu::PMap::CartridgeSram.IsBetween(address))
			return memory.Sram()[address - Mmu::PMap::CartridgeSram.base];
		else N64Logger::Abort();
		return {};
	}

	void PI::dmaWrite(N64System& n64, uint32 wrLen)
	{
		m_wrLen = wrLen;

		const uint32 dramAddr = m_dramAddr & 0x7FFFFE;
		const uint32 cartAddr = m_cartAddr & 0xFFFFFFFE;
		const uint32 transferLength = [&]()
		{
			uint32 value = (wrLen & 0x00FF'FFFF) + 1;
			if (dramAddr & 0x7) value -= dramAddr & 0x7;
			return value;
		}();

		N64_TRACE(Mmio, U"start PI DMA write: {} bytes {:08X} -> {:08X}"_fmt(transferLength, cartAddr, dramAddr));

		if (cartAddr < Mmu::PMap::N64DdIplRom.base)
			N64Logger::Abort(U"PI DMA transfer card address is out of range: {}"_fmt(cartAddr));

		for (uint32 i = 0; i < transferLength; ++i)
		{
			// データ転送
			n64.GetMemory().Rdram()[EndianAddress<uint8>(dramAddr + i)] = readCartridge(n64.GetMemory(), cartAddr + i);
		}

		// 対応する再コンパイル済みキャッシュも無効に
		n64.GetCpu().RecompiledCache().CheckInvalidatePageBetween(
			PAddr32(EndianAddress<uint8>(dramAddr)), PAddr32(EndianAddress<uint8>(dramAddr + transferLength - 1)));

		m_status.DmaBusy().Set(true);
		m_status.IoBusy().Set(true);

		n64.GetScheduler().ScheduleEvent(transferLength / 8, [&n64, this]()
		{
			N64_TRACE(Mmio, U"completed PI DMA write");

			m_status.DmaBusy().Set(false);
			m_status.IoBusy().Set(false);

			InterruptRaise<Interruption::PI>(n64);
		});
	}
}
