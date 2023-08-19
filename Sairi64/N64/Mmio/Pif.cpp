#include "stdafx.h"
#include "Pif.h"

#include "Pif_executeRomHle.h"
#include "N64/Mmu.h"
#include "N64/N64System.h"
#include "N64/N64Logger.h"

namespace N64::Mmio
{
	void Pif::ExecuteRom(N64System& n64)
	{
		switch (n64.GetMemory().GetRom().Cic())
		{
		case CicType::CIC_UNKNOWN:
			N64Logger::Abort();
			break;
		case CicType::CIC_NUS_6101:
		case CicType::CIC_NUS_7102:
		case CicType::CIC_NUS_6102_7101:
		case CicType::CIC_NUS_6103_7103:
			Mmu::WritePaddr32(n64, PAddr32(0x318), RdramSize_0x00800000);
			break;
		case CicType::CIC_NUS_6105_7105:
			Mmu::WritePaddr32(n64, PAddr32(0x3F0), RdramSize_0x00800000);
			break;
		case CicType::CIC_NUS_6106_7106:
			break;
		default: ;
		}
		Pif_executeRomHle(n64);
	}
}
