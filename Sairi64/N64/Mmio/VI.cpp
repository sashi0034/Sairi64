#include "stdafx.h"
#include "VI.h"

#include "N64/Interrupt.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Mmio
{
	uint32 VI::Read32(PAddr32 paddr) const
	{
		switch (paddr)
		{
		case ViAddress::Control_0x04400000:
			return m_control;
		case ViAddress::Origin_0x04400004:
			return m_origin;
		case ViAddress::Width_0x04400008:
			return m_width;
		case ViAddress::VInterrupt_0x0440000C:
			return m_vInterrupt;
		case ViAddress::VCurrent_0x04400010:
			return m_vCurrent; // ?
		case ViAddress::Burst_0x04400014:
			return m_burst;
		case ViAddress::VSync_0x04400018:
			return m_vSync;
		case ViAddress::HSync_0x0440001C:
			return m_hSync;
		case ViAddress::HSyncLeap_0x04400020:
			return m_hSyncLeap;
		case ViAddress::HVideo_0x04400024:
			return m_hVideo;
		case ViAddress::VVideo_0x04400028:
			return m_vVideo;
		case ViAddress::VBurst_0x0440002C:
			return m_vBurst;
		case ViAddress::XScale_0x04400030:
			return m_xScale;
		case ViAddress::YScale_0x04400034:
			return m_yScale;
		default: break;
		}

		N64Logger::Abort(U"unsupported vi read: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void VI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case ViAddress::Control_0x04400000:
			m_control = {value};
			m_numFields = m_control.Serrate() ? 2 : 1;
			return;
		case ViAddress::Origin_0x04400004: {
			const uint32 masked = value & 0xFFFFFF;
			if (m_origin != masked) m_swaps++;
			m_origin = masked;
			return;
		}
		case ViAddress::Width_0x04400008:
			m_width = value & 0x7FF;
			return;
		case ViAddress::VInterrupt_0x0440000C:
			// When VCurrent reaches this half-line number, a VI Interrupt is triggered
			m_vInterrupt = value & 0x3FF;
			return;
		case ViAddress::VCurrent_0x04400010:
			InterruptLower<Interruption::VI>(n64);
			return;
		case ViAddress::Burst_0x04400014:
			m_vBurst = value;
			return;
		case ViAddress::VSync_0x04400018:
			m_vSync = value & 0x3FF;
			m_numHalfLines = m_vSync >> 1;
			m_cyclesPerHalfLine = GetCpuCyclesPerFrame_1562500(n64.GetMemory().IsRomPal()) / m_numHalfLines;
			return;
		case ViAddress::HSync_0x0440001C:
			m_hSync = value & 0x3FF;
			return;
		case ViAddress::HSyncLeap_0x04400020:
			m_hSync = value;
			return;
		case ViAddress::HVideo_0x04400024:
			m_hVideo = {value};
			return;
		case ViAddress::VVideo_0x04400028:
			m_vVideo = {value};
			return;
		case ViAddress::VBurst_0x0440002C:
			m_vBurst = value;
			return;
		case ViAddress::XScale_0x04400030:
			m_xScale = value;
			return;
		case ViAddress::YScale_0x04400034:
			m_yScale = value;
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported vi write: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
	}
}
