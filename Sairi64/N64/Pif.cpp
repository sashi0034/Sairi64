#include "stdafx.h"
#include "Pif.h"

#include "Mmu.h"
#include "N64System.h"
#include "N64Logger.h"

namespace N64
{
	void executePifRomHle_switchCic(N64System& n64, bool pal, CicType cic)
	{
		auto&& cpu = n64.GetCpu();
		auto&& gpr = cpu.GetGpr();

		switch (cic)
		{
		case CicType::CIC_NUS_6101:
			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000000);
			gpr.Write(2, 0xFFFFFFFFDF6445CC);
			gpr.Write(3, 0xFFFFFFFFDF6445CC);
			gpr.Write(4, 0x00000000000045CC);
			gpr.Write(5, 0x0000000073EE317A);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0xFFFFFFFFC7601FAC);
			gpr.Write(13, 0xFFFFFFFFC7601FAC);
			gpr.Write(14, 0xFFFFFFFFB48E2ED6);
			gpr.Write(15, 0xFFFFFFFFBA1A7D4B);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000001);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(23, 0x0000000000000001);
			gpr.Write(24, 0x0000000000000002);
			gpr.Write(25, 0xFFFFFFFF905F4718);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001550);

			cpu.SetLo(0xFFFFFFFFBA1A7D4B);
			cpu.SetHi(0xFFFFFFFF997EC317);

			break;
		case CicType::CIC_NUS_7102:
			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000001);
			gpr.Write(2, 0x000000001E324416);
			gpr.Write(3, 0x000000001E324416);
			gpr.Write(4, 0x0000000000004416);
			gpr.Write(5, 0x000000000EC5D9AF);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0x00000000495D3D7B);
			gpr.Write(13, 0xFFFFFFFF8B3DFA1E);
			gpr.Write(14, 0x000000004798E4D4);
			gpr.Write(15, 0xFFFFFFFFF1D30682);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000000);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(22, 0x000000000000003F);
			gpr.Write(23, 0x0000000000000007);
			gpr.Write(24, 0x0000000000000000);
			gpr.Write(25, 0x0000000013D05CAB);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001554);

			cpu.SetLo(0xFFFFFFFFF1D30682);
			cpu.SetHi(0x0000000010054A98);

			break;
		case CicType::CIC_NUS_6102_7101:
			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000001);
			gpr.Write(2, 0x000000000EBDA536);
			gpr.Write(3, 0x000000000EBDA536);
			gpr.Write(4, 0x000000000000A536);
			gpr.Write(5, 0xFFFFFFFFC0F1D859);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0xFFFFFFFFED10D0B3);
			gpr.Write(13, 0x000000001402A4CC);
			gpr.Write(14, 0x000000002DE108EA);
			gpr.Write(15, 0x000000003103E121);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000001);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(23, 0x0000000000000000);
			gpr.Write(24, 0x0000000000000000);
			gpr.Write(25, 0xFFFFFFFF9DEBB54F);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001550);

			cpu.SetLo(0x000000003103E121);
			cpu.SetHi(0x000000003FC18657);

			if (pal)
			{
				gpr.Write(20, 0x0000000000000000);
				gpr.Write(23, 0x0000000000000006);
				gpr.Write(31, 0xFFFFFFFFA4001554);
			}
			break;
		case CicType::CIC_NUS_6103_7103:
			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000001);
			gpr.Write(2, 0x0000000049A5EE96);
			gpr.Write(3, 0x0000000049A5EE96);
			gpr.Write(4, 0x000000000000EE96);
			gpr.Write(5, 0xFFFFFFFFD4646273);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0xFFFFFFFFCE9DFBF7);
			gpr.Write(13, 0xFFFFFFFFCE9DFBF7);
			gpr.Write(14, 0x000000001AF99984);
			gpr.Write(15, 0x0000000018B63D28);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000001);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(23, 0x0000000000000000);
			gpr.Write(24, 0x0000000000000000);
			gpr.Write(25, 0xFFFFFFFF825B21C9);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001550);

			cpu.SetLo(0x0000000018B63D28);
			cpu.SetHi(0x00000000625C2BBE);

			if (pal)
			{
				gpr.Write(20, 0x0000000000000000);
				gpr.Write(23, 0x0000000000000006);
				gpr.Write(31, 0xFFFFFFFFA4001554);
			}
			break;
		case CicType::CIC_NUS_6105_7105:
			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000000);
			gpr.Write(2, 0xFFFFFFFFF58B0FBF);
			gpr.Write(3, 0xFFFFFFFFF58B0FBF);
			gpr.Write(4, 0x0000000000000FBF);
			gpr.Write(5, 0xFFFFFFFFDECAAAD1);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0xFFFFFFFF9651F81E);
			gpr.Write(13, 0x000000002D42AAC5);
			gpr.Write(14, 0x00000000489B52CF);
			gpr.Write(15, 0x0000000056584D60);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000001);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(23, 0x0000000000000000);
			gpr.Write(24, 0x0000000000000002);
			gpr.Write(25, 0xFFFFFFFFCDCE565F);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001550);

			cpu.SetLo(0x0000000056584D60);
			cpu.SetHi(0x000000004BE35D1F);

			if (pal)
			{
				gpr.Write(20, 0x0000000000000000);
				gpr.Write(23, 0x0000000000000006);
				gpr.Write(31, 0xFFFFFFFFA4001554);
			}

			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x00), 0x3C0DBFC0);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x04), 0x8DA807FC);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x08), 0x25AD07C0);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x0C), 0x31080080);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x10), 0x5500FFFC);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x14), 0x3C0DBFC0);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x18), 0x8DA80024);
			Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::SpImem.base + 0x1C), 0x3C0BB000);

			break;
		case CicType::CIC_NUS_6106_7106:

			gpr.Write(0, 0x0000000000000000);
			gpr.Write(1, 0x0000000000000000);
			gpr.Write(2, 0xFFFFFFFFA95930A4);
			gpr.Write(3, 0xFFFFFFFFA95930A4);
			gpr.Write(4, 0x00000000000030A4);
			gpr.Write(5, 0xFFFFFFFFB04DC903);
			gpr.Write(6, 0xFFFFFFFFA4001F0C);
			gpr.Write(7, 0xFFFFFFFFA4001F08);
			gpr.Write(8, 0x00000000000000C0);
			gpr.Write(9, 0x0000000000000000);
			gpr.Write(10, 0x0000000000000040);
			gpr.Write(11, 0xFFFFFFFFA4000040);
			gpr.Write(12, 0xFFFFFFFFBCB59510);
			gpr.Write(13, 0xFFFFFFFFBCB59510);
			gpr.Write(14, 0x000000000CF85C13);
			gpr.Write(15, 0x000000007A3C07F4);
			gpr.Write(16, 0x0000000000000000);
			gpr.Write(17, 0x0000000000000000);
			gpr.Write(18, 0x0000000000000000);
			gpr.Write(19, 0x0000000000000000);
			gpr.Write(20, 0x0000000000000001);
			gpr.Write(21, 0x0000000000000000);
			gpr.Write(23, 0x0000000000000000);
			gpr.Write(24, 0x0000000000000002);
			gpr.Write(25, 0x00000000465E3F72);
			gpr.Write(26, 0x0000000000000000);
			gpr.Write(27, 0x0000000000000000);
			gpr.Write(28, 0x0000000000000000);
			gpr.Write(29, 0xFFFFFFFFA4001FF0);
			gpr.Write(30, 0x0000000000000000);
			gpr.Write(31, 0xFFFFFFFFA4001550);

			cpu.SetLo(0x000000007A3C07F4);
			cpu.SetHi(0x0000000023953898);

			if (pal)
			{
				gpr.Write(20, 0x0000000000000000);
				gpr.Write(23, 0x0000000000000006);
				gpr.Write(31, 0xFFFFFFFFA4001554);
			}
			break;
		default:
			N64Logger::Abort(U"boot failed due to unsupported cic");
			break;
		}
	}

	void executePifRomHle(N64System& n64)
	{
		const bool pal = false; // TODO

		auto&& rom = n64.GetMemory().GetRom();
		const uint32 cicSeed = rom.CicSeed();
		Mmu::WritePaddr32(n64, PAddr32(Mmu::PMap::PifRam.base + 0x24), cicSeed);

		// CICごとに処理を分岐
		executePifRomHle_switchCic(n64, pal, rom.Cic());

		// CPU適用
		n64.GetCpu().GetGpr().Write(22, (cicSeed >> 8) & 0xFF);
		n64.GetCpu().GetPc().Reset(0xA4000040);

		// TODO: COP0初期化

		// TODO: fix?
		// Mmu::WritePaddr32(n64, PAddr32(0x04300004), 0x01010101);

		// ROMの最初0x1000バイトをSP DMEMにコピー
		std::copy_n(rom.Data().begin(), 0x1000, n64.GetRsp().Dmem().begin());
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
