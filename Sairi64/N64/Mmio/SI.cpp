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
		case SiAddress::PifAdRd64B_0x04800004: // fallthrough
		case SiAddress::PifAddrWr64B_0x04800010:
			return m_pifAddr;
		case SiAddress::Status_0x04800018:
			return uint32()
				| SiStatus32(m_status).DmaBusy()
				| (0 << 1)
				| (0 << 3)
				| (n64.GetMI().GetInterrupt().Si() << 12);
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
			startDma<DmaType::PifToDram>(n64, value);
			return;
		case SiAddress::PifAddrWr64B_0x04800010:
			startDma<DmaType::DramToPif>(n64, value);
			return;
		case SiAddress::Status_0x04800018:
			m_status.Interrupt().Set(false);
			InterruptLower<Interruption::SI>(n64);
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported si write: paddr={:08X}, value={:08X}"_fmt(static_cast<uint32>(paddr), value));
	}

	constexpr int dmaDelay_131072 = 65536 * 2;

	template <SI::DmaType dma>
	void SI::startDma(N64System& n64, uint32 pifAddr)
	{
		N64_TRACE(
			dma == DmaType::DramToPif ? U"SI DMA start DRAM to PIF" :
			dma == DmaType::PifToDram ? U"SI DMA start PIF to DRAM" : U"");

		m_pifAddr = pifAddr & 0x1FFFFFFF;
		m_dmaRunning++;

		n64.GetScheduler().ScheduleEvent(dmaDelay_131072, [this, &n64, pifAddr]()
		{
			// DMA結果反映
			achieveDma<dma>(n64, pifAddr);
		});
	}

	template <SI::DmaType dma>
	void SI::achieveDma(N64System& n64, uint32 pifAddr)
	{
		N64_TRACE(
			dma == DmaType::DramToPif ? U"SI DMA achieved DRAM to PIF" :
			dma == DmaType::PifToDram ? U"SI DMA achieved PIF to DRAM" : U"");

		m_dmaRunning--;

		static_assert(
			dma == DmaType::PifToDram ||
			dma == DmaType::DramToPif);

		if constexpr (dma == DmaType::PifToDram)
		{
			m_pif.ProcessCommands();
			for (int i = 0; i < 64; ++i)
				n64.GetMemory().Rdram()[m_dramAddr + i] = m_pif.Ram()[i]; // もしかしたらSI_PIF_ADDRレジスタを利用?
		}
		else if constexpr (dma == DmaType::DramToPif)
		{
			for (int i = 0; i < 64; ++i)
				m_pif.Ram()[i] = n64.GetMemory().Rdram()[m_dramAddr + i];
			m_pif.ProcessCommands();
		}

		InterruptRaise<Interruption::SI>(n64);
	}
}
