#include "stdafx.h"
#include "AI.h"

#include "N64/Interrupt.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Mmio
{
	// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/mmio/AI.cpp#L20
	uint32 AI::Read32(PAddr32 paddr) const
	{
		if (paddr == 0x0450000C)
		{
			uint32 value = 0;
			value |= (m_dmaCount > 1);
			value |= 1 << 20;
			value |= 1 << 24;
			value |= (m_dmaEnable << 25);
			value |= (m_dmaCount > 0) << 30;
			value |= (m_dmaCount > 1) << 31;
			return value;
		}

		return m_dmaLength[0];
	}

	// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/mmio/AI.cpp#L35
	void AI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case AiAddress::DramAddr_0x04500000:
			if (m_dmaCount < 2)
			{
				m_dmaAddr[m_dmaCount] = value & 0xFFFFFF & ~7;
			}
			return;
		case AiAddress::Length_0x04500004:
			if (m_dmaCount < 2)
			{
				if (m_dmaCount == 0) InterruptRaise<Interruption::AI>(n64);
				const uint32 length = (value & 0x3FFFF) & ~7;
				m_dmaLength[m_dmaCount] = length;
				m_dmaCount++;
			}
			return;
		case AiAddress::Control_0x04500008:
			m_dmaEnable = value & 1;
			return;
		case AiAddress::Status_0x0450000C:
			InterruptLower<Interruption::AI>(n64);
			return;
		case AiAddress::DacRate_0x04500010:
			writeDacRate(n64, value);
			return;
		case AiAddress::BitRate_0x04500014:
			m_bitRate = value & 0xF;
			m_dac.precision = m_bitRate + 1;
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported ai write: {:08X}"_fmt(static_cast<uint32>(paddr)));
	}

	void AI::writeDacRate(N64System& n64, uint32 value)
	{
		m_dacRate = value & 0x3FFF;
		const auto oldFreq = m_dac.freq;
		m_dac.freq = std::max(1u, GetVideoFreq_48681812(n64.GetMemory().IsRomPal()) / (m_dacRate + 1));
		m_dac.period = CpuFreq_93750000 / m_dac.freq;
		if (oldFreq != m_dac.freq)
		{
			// TODO
		}
	}
}
