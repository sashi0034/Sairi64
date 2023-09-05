// These test cases are sourced from the [r64emu](https://github.com/rasky/r64emu) project.
// I would like to thank the original authors and contributors.

#include <Catch2/catch.hpp>

#include "N64/N64Singleton.h"
String testDir = U"tests/rsp/";
constexpr int maxCycles = 100000;

using namespace N64;

#define WAIT_ENABLE true

#if 1
bool inspectResult(Rsp& rsp, BinaryReader goldenBinary)
{
	const uint32 goldenSize = goldenBinary.size();
	Array<uint8> goldenData(goldenSize);
	goldenBinary.read(goldenData.data(), goldenSize);

	// @formatter:off
	Console.writeln(U"         [actual]                                                [expected]                                     ");
	Console.writeln(U"         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F         00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	// @formatter:on

	bool ok = true;
	for (uint32 i = 0; i < goldenSize; i += 16)
	{
		const uint32 checkBaseAddr = i + 0x800;
		if (checkBaseAddr >= Rsp_detail::SpDmemSize_0x1000) break;
		Console.write(U"0x{:04X}:  "_fmt(checkBaseAddr));

		String actualLine{};
		String expectedLine{};
		String errorLine{};
		for (int x = 0; x < 16; ++x)
		{
			if (i + x >= goldenSize) break;
			const uint8 actual = rsp.Dmem()[N64::EndianAddress<uint8>(checkBaseAddr + x)];
			const uint8 expected = goldenData[i];
			actualLine += U"{:02X} "_fmt(actual);
			expectedLine += U"{:02X} "_fmt(expected);
			if (actual != expected)
			{
				ok = false;
				errorLine += U"^^ ";
			}
			else
			{
				errorLine += U"  " " ";
			}
		}
		Console.write(U"{}        {}\n"_fmt(actualLine, expectedLine));
		if (errorLine.includes(U"^")) Console.write(U"         {}        {}\n"_fmt(errorLine, errorLine));
	}
	if (ok) Console.writeln(U"\npassed test!\n");
	else Console.writeln(U"\nfailed test!\n");
	return ok;
}

// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/tests/test_rsp.c#L44
bool rspTest(const String& fileName)
{
	const auto n64 = std::make_unique<N64Singleton>();
	N64System& n64System = n64->GetSystem();
	auto&& rsp = n64System.GetRsp();

	Console.writeln(U"start rsp test: {}"_fmt(fileName));

	BinaryReader rspBinary{testDir + fileName + U".rsp"};
	rspBinary.read(rsp.Imem().data(), rspBinary.size());
	Utils::ByteSwapWordArray(rsp.Imem());

	BinaryReader dmemBinary{testDir + fileName + U".input"};
	dmemBinary.read(rsp.Dmem().data(), dmemBinary.size());
	Utils::ByteSwapWordArray(rsp.Dmem());

	rsp.Status().Halt().Set(false);
	int cycles = 0;
	while (true)
	{
		if (rsp.IsHalted()) break;
		cycles += rsp.Step(n64System);
		if (cycles > maxCycles)
		{
			Console.writeln(U"program will not finish!");
			return false;
		}
	}

	const BinaryReader goldenBinary{testDir + fileName + U".golden"};

	const bool ok = inspectResult(rsp, goldenBinary);

#if WAIT_ENABLE
	Utils::WaitAnyKeyOnConsole();
#endif

	return ok;
}

TEST_CASE("RspTest")
{
	REQUIRE(rspTest(U"lqv_sqv"));
	REQUIRE(rspTest(U"vadd"));
	REQUIRE(rspTest(U"lhv_shv"));
	REQUIRE(rspTest(U"vrcp"));
	REQUIRE(rspTest(U"vlt"));
	REQUIRE(rspTest(U"llv_slv"));
	REQUIRE(rspTest(U"vcr"));
	REQUIRE(rspTest(U"vmadh"));
	REQUIRE(rspTest(U"vmadl"));
	REQUIRE(rspTest(U"vsucb"));
	REQUIRE(rspTest(U"lsv_ssv"));
	REQUIRE(rspTest(U"vrcpl"));
	REQUIRE(rspTest(U"lpv_spv"));
	REQUIRE(rspTest(U"ldv_sdv"));
	REQUIRE(rspTest(U"mtc2"));
	REQUIRE(rspTest(U"luv_suv"));
	REQUIRE(rspTest(U"compelt"));
	REQUIRE(rspTest(U"vge"));
	REQUIRE(rspTest(U"vmrg"));
	REQUIRE(rspTest(U"vsub"));
	REQUIRE(rspTest(U"swv"));
	REQUIRE(rspTest(U"vmadm"));
	REQUIRE(rspTest(U"vch"));
	REQUIRE(rspTest(U"vmulu"));
	REQUIRE(rspTest(U"vmacu"));
	REQUIRE(rspTest(U"vmulf"));
	REQUIRE(rspTest(U"vcl"));
	REQUIRE(rspTest(U"stv"));
	REQUIRE(rspTest(U"lfv_sfv"));
	REQUIRE(rspTest(U"vaddc"));
	REQUIRE(rspTest(U"vmadn"));
	REQUIRE(rspTest(U"vsubc"));
	REQUIRE(rspTest(U"lrv_srv"));
	REQUIRE(rspTest(U"vmudl"));
	REQUIRE(rspTest(U"vne"));
	REQUIRE(rspTest(U"vrsq"));
	REQUIRE(rspTest(U"veq"));
	REQUIRE(rspTest(U"memaccess"));
	REQUIRE(rspTest(U"vmacf"));
	REQUIRE(rspTest(U"vmudh"));
	REQUIRE(rspTest(U"vmudn"));
	REQUIRE(rspTest(U"vmudm"));
	REQUIRE(rspTest(U"ltv"));
	REQUIRE(rspTest(U"vsubb"));
	REQUIRE(rspTest(U"lbv_sbv"));
	REQUIRE(rspTest(U"mfc2"));
	REQUIRE(rspTest(U"vlogical"));
}
#endif
