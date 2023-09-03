// These test cases are sourced from the [r64emu](https://github.com/rasky/r64emu) project.
// I would like to thank the original authors and contributors.

#include "N64/Memory.h"
#include "N64/N64Frame.h"
#include "N64/N64Singleton.h"
String testDir = U"tests/rsp";

std::array testFiles{
	U"vadd",
	U"lhv_shv",
	U"vrcp",
	U"vlt",
	U"llv_slv",
	U"vcr",
	U"vmadh",
	U"vmadl",
	U"vsucb",
	U"lsv_ssv",
	U"vrcpl",
	U"lpv_spv",
	U"README",
	U"ldv_sdv",
	U"mtc2",
	U"luv_suv",
	U"compelt",
	U"vge",
	U"vmrg",
	U"vsub",
	U"swv",
	U"vmadm",
	U"vch",
	U"vmulu",
	U"vmacu",
	U"vmulf",
	U"vcl",
	U"stv",
	U"lfv_sfv",
	U"vaddc",
	U"vmadn",
	U"vsubc",
	U"lrv_srv",
	U"lqv_sqv",
	U"vmudl",
	U"vne",
	U"vrsq",
	U"veq",
	U"memaccess",
	U"vmacf",
	U"vmudh",
	U"vmudn",
	U"vmudm",
	U"ltv",
	U"vsubb",
	U"lbv_sbv",
	U"mfc2",
	U"vlogical",
};

using namespace N64;

bool rspTest(const String& fileName)
{
	const auto n64 = std::make_unique<N64Singleton>();
	N64System& n64System = n64->GetSystem();
	auto&& rsp = n64System.GetRsp();

	BinaryReader rspBinary{fileName + U".rsp"};
	rspBinary.read(rsp.Imem().data(), rspBinary.size());
	Utils::ByteSwapArrayData(rsp.Imem());

	// BinaryReader dmemBinary{fileName + U".input"};
	// dmemBinary.read(rsp.Imem().data(), dmemBinary.size());

	String goldenPath = fileName + U".golden";
	return {};
}
