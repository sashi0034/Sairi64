// These test cases are sourced from the [r64emu](https://github.com/rasky/r64emu) project.
// I would like to thank the original authors and contributors.

#include <Catch2/catch.hpp>

#include "N64/N64Singleton.h"
String testDir = U"tests/rsp/";
constexpr int maxCycles = 100000;

using namespace N64;

#define WAIT_ENABLE true

#if 1
bool inspectResult(Rsp& rsp, const Array<uint8>& goldenData)
{
	const auto goldenSize = goldenData.size();

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
			const uint8 expected = goldenData[i + x];
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

#if WAIT_ENABLE
	if (ok == false) Utils::WaitAnyKeyOnConsole();
#endif

	return ok;
}

bool runRsp(N64System& n64System, Rsp& rsp)
{
	rsp.Status().Halt().Set(false);
	rsp.GetPc().Reset(0);
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
	return true;
}

// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/tests/test_rsp.c#L44
bool rspTest(const String& fileName)
{
	const auto n64 = std::make_unique<N64Singleton>();
	N64System& n64System = n64->GetSystem();
	auto&& rsp = n64System.GetRsp();

	Console.writeln(U"start rsp test: {}"_fmt(fileName));

	// 命令読み込み
	BinaryReader rspBinary{testDir + fileName + U".rsp"};
	rspBinary.read(rsp.Imem().data(), rspBinary.size());
	Utils::ByteSwapWordArray(rsp.Imem());

	const TOMLReader toml{testDir + U"input/" + fileName + U".toml"};
	const size_t numTestcases = toml[U"test"].arrayCount();

	BinaryReader goldenBinary{testDir + fileName + U".golden"};
	const size_t goldenSingle = goldenBinary.size() / numTestcases;
	BinaryReader inputBinary{testDir + fileName + U".input"};
	const size_t inputSingle = inputBinary.size() / numTestcases;

	bool ok = true;
	for (int i = 0; i < numTestcases; ++i)
	{
		Console.writeln(U"\nnext subset [ {} / {} ]\n"_fmt(i, numTestcases - 1));

		inputBinary.read(rsp.Dmem().data(), inputSingle * i, inputSingle);
		// Utils::ByteSwapWordArray(rsp.Dmem());

		Array<uint8> golden(goldenSingle);
		goldenBinary.read(golden.data(), goldenSingle * i, goldenSingle);

		if (runRsp(n64System, rsp) == false) return false;

		Console.writeln(U"\nfinished subset [ {} / {} ]\n"_fmt(i, numTestcases - 1));
		ok &= inspectResult(rsp, golden);
	}

	Console.writeln(U"*-*-*-*-*-*-*-*-*-*-*-* [ finished {}: ok={} ] *-*-*-*-*-*-*-*-*-*-*-*\n"_fmt(fileName, ok));

	return ok;
}

TEST_CASE("RspTest")
{
	// REQUIRE(rspTest(U"lqv_sqv")); // OK
	// REQUIRE(rspTest(U"vadd")); // OK
	// REQUIRE(rspTest(U"lhv_shv")); // OK
	// REQUIRE(rspTest(U"vrcp")); // OK
	// REQUIRE(rspTest(U"vaddc")); // OK
	// REQUIRE(rspTest(U"vsub")); // OK
	// REQUIRE(rspTest(U"vsubc")); // OK
	// REQUIRE(rspTest(U"lrv_srv")); // OK
	// REQUIRE(rspTest(U"vmudh")); // OK
	// REQUIRE(rspTest(U"vmudl")); // OK
	// REQUIRE(rspTest(U"vmudm")); // OK
	// REQUIRE(rspTest(U"vmudn")); // OK
	// REQUIRE(rspTest(U"vmadh")); // OK
	// REQUIRE(rspTest(U"vmadl")); // OK
	// REQUIRE(rspTest(U"vmadm")); // OK
	// REQUIRE(rspTest(U"vmadn")); // OK
	// REQUIRE(rspTest(U"ldv_sdv")); // OK
	// REQUIRE(rspTest(U"memaccess")); // OK
	// REQUIRE(rspTest(U"ltv")); // OK
	// REQUIRE(rspTest(U"stv")); // OK
	// REQUIRE(rspTest(U"mfc2")); // OK
	// REQUIRE(rspTest(U"mtc2")); // OK
	// REQUIRE(rspTest(U"vmulf")); // OK
	// REQUIRE(rspTest(U"vmulu")); // OK
	// REQUIRE(rspTest(U"vmacf")); // OK
	// REQUIRE(rspTest(U"vmacu")); // OK
	// REQUIRE(rspTest(U"vlt")); // OK
	// REQUIRE(rspTest(U"veq")); // OK
	// REQUIRE(rspTest(U"vne")); // OK
	// REQUIRE(rspTest(U"vge")); // OK
	// REQUIRE(rspTest(U"vcl")); // OK
	// REQUIRE(rspTest(U"vch")); // OK
	// REQUIRE(rspTest(U"vcr")); // OK
	// REQUIRE(rspTest(U"vmrg")); // OK
	// REQUIRE(rspTest(U"vlogical")); // OK
	// REQUIRE(rspTest(U"compelt")); // OK
	// REQUIRE(rspTest(U"llv_slv")); // OK
	// REQUIRE(rspTest(U"vsucb")); // OK
	// REQUIRE(rspTest(U"lsv_ssv")); // OK

	REQUIRE(rspTest(U"vrcpl"));
	REQUIRE(rspTest(U"lpv_spv"));
	REQUIRE(rspTest(U"luv_suv"));
	REQUIRE(rspTest(U"swv"));
	REQUIRE(rspTest(U"lfv_sfv"));
	REQUIRE(rspTest(U"vrsq"));
	REQUIRE(rspTest(U"vsubb"));
	REQUIRE(rspTest(U"lbv_sbv"));
}
#endif
