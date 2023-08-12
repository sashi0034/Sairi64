#include "stdafx.h"
#include "Pif.h"

#include "Mmu.h"
#include "N64System.h"
#include "N64Logger.h"

namespace N64
{
	void executePifRomHle(N64System& n64)
	{
		bool pal = false; // TODO

		const uint32 cicSeed = n64.GetMemory().GetRom().CicSeed();
		Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::PifRam.base + 0x24), cicSeed);

		// TODO: GPRなど


		// CPU設定
		n64.GetCpu().GetGpr().Write(22, )
		n64.GetCpu().GetPc().Reset(0xA4000040);

		// TODO: COP0初期化
	}

	void Pif::ExecutePifRom(N64System& n64)
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
		executePifRomHle(n64);
	}
}
