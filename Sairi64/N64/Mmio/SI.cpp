#include "stdafx.h"
#include "N64/N64System.h"
#include "N64/N64Logger.h"

namespace N64::Mmio
{
	uint32 SI::Read32(N64System& n64, PAddr32 paddr) const
	{
		switch (paddr)
		{
		case SiAddress::DramAddr_0x04800000:
			return m_dramAddr;
		case SiAddress::PifAdRd64B_0x04800004:
		case SiAddress::PifAddrWr64B_0x04800010:
			return m_pifAddr;
		case SiAddress::Status_0x04800018:
			// uint32 status{};
		// TODO: MIが必要
			break;
		default: break;
		}

		N64Logger::Abort(U"unsupported si read: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void SI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case SiAddress::DramAddr_0x04800000:
			m_dramAddr = value & RdramSizeMask_0x007FFFFF;
			return;
		case SiAddress::PifAdRd64B_0x04800004:
			m_pifAddr = value & 0x1FFFFFFF;
		// TODO: DMA
			break;
		case SiAddress::PifAddrWr64B_0x04800010:
			// TODO: DMA
			break;
		case SiAddress::Status_0x04800018:
			// TODO: MIを作ってから
			m_status.Interrupt().Set(false);
		// TODO: 割り込み部分
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported si write: paddr={:08X}, value={:08X}"_fmt(static_cast<uint32>(paddr), value));
	}
}
