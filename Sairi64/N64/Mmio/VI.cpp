#include "stdafx.h"
#include "VI.h"

#include "N64/N64Logger.h"

namespace N64::Mmio
{
	uint32 VI::Read32(PAddr32 paddr) const
	{
		switch (paddr)
		{
		case ViAddress::Status_0x04400000:
			break;
		case ViAddress::Origin_0x04400004:
			break;
		case ViAddress::Width_0x04400008:
			break;
		case ViAddress::VInterrupt_0x0440000C:
			break;
		case ViAddress::VCurrent_0x04400010:
			break;
		case ViAddress::Burst_0x04400014:
			break;
		case ViAddress::VSync_0x04400018:
			break;
		case ViAddress::HSync_0x0440001C:
			break;
		case ViAddress::HSyncLeap_0x04400020:
			break;
		case ViAddress::HVideo_0x04400024:
			break;
		case ViAddress::VVideo_0x04400028:
			break;
		case ViAddress::VBurst_0x0440002C:
			break;
		case ViAddress::XScale_0x04400030:
			break;
		case ViAddress::YScale_0x04400034:
			break;
		default: break;
		}

		N64Logger::Abort(U"unsupported vi read: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void VI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case ViAddress::Status_0x04400000:
			break;
		case ViAddress::Origin_0x04400004:
			break;
		case ViAddress::Width_0x04400008:
			break;
		case ViAddress::VInterrupt_0x0440000C:
			break;
		case ViAddress::VCurrent_0x04400010:
			break;
		case ViAddress::Burst_0x04400014:
			break;
		case ViAddress::VSync_0x04400018:
			break;
		case ViAddress::HSync_0x0440001C:
			break;
		case ViAddress::HSyncLeap_0x04400020:
			break;
		case ViAddress::HVideo_0x04400024:
			break;
		case ViAddress::VVideo_0x04400028:
			break;
		case ViAddress::VBurst_0x0440002C:
			break;
		case ViAddress::XScale_0x04400030:
			break;
		case ViAddress::YScale_0x04400034:
			break;
		default: break;
		}

		N64Logger::Abort(U"unsupported vi write: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
	}
}
