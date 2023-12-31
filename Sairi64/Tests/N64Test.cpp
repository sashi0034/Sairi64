﻿#include "stdafx.h"

#include <Catch2/catch.hpp>


#include "N64/Mmu.h"
#include "N64/N64Frame.h"
#include "N64/N64Singleton.h"
#include "Utils/Util.h"

#define WAIT_ENABLE false

#if 0
namespace Tests
{
	// https://github.com/Dillonb/n64-tests
	bool dillon64Test(const String& fileName)
	{
		using namespace N64;

		const auto n64 = std::make_unique<N64Singleton>();
		N64System& n64System = n64->GetSystem();
		N64Frame n64Frame{};

		Console.writeln(U"start test rom: {}"_fmt(fileName));

		const N64Config config{
			.rom = {.filePath = U"asset/rom/dillonb-n64-tests/{}"_fmt(fileName)},
			.boot = {
				.executePifRom = false
			}
		};

		// テスト用の初期化
		n64Frame.Init(n64System, config);
		for (uint32 i = 0; i < 0x100000; i += 4)
		{
			const uint32 data = Mmu::ReadPaddr32(n64System, PAddr32(0x10001000 + i));
			Mmu::WritePaddr32(n64System, PAddr32(0x00001000 + i), data);
		}
		n64System.GetCpu().GetPc().Change64(0x80001000);

		// N64コンソール実行
		constexpr int targetR30 = 30;
		n64Frame.RunOnConsole(n64System, config, [&]()
		{
			return n64System.GetCpu().GetGpr().Read(targetR30) != 0;
		});

		const sint32 expected_minus1 = n64System.GetCpu().GetGpr().Read(targetR30);

		// キー入力待機
		Console.writeln(U"end test rom: {}\ngpr[31]={:08X}\n"_fmt(fileName, static_cast<uint32>(expected_minus1)));
#if WAIT_ENABLE
		Utils::WaitAnyKeyOnConsole();
#endif

		return expected_minus1 == -1;
	}

	void concludeStopwatch(Stopwatch& stopwatch)
	{
		stopwatch.pause();
		double time = stopwatch.sF();
		Console.writeln(U"tests completed in {:.4f} sec"_fmt(time));
	}

	// https://github.com/Dillonb/n64-tests/tree/master/src
	TEST_CASE("N64Test_Dillon64Test")
	{
		Stopwatch stopwatch{StartImmediately::Yes};

		REQUIRE(dillon64Test(U"basic_simpleboot.z64")); // OK

		REQUIRE(dillon64Test(U"addiu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"addu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"and_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"andi_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"daddiu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"dsll32_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"dsll_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"dsllv_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"dsra32_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"nor_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"or_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"ori_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"sll_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"sllv_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"slt_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"slti_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"sltiu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"sltu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"sra_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"srav_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"srl_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"srlv_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"subu_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"xor_simpleboot.z64")); // OK
		REQUIRE(dillon64Test(U"xori_simpleboot.z64")); // OK

		concludeStopwatch(stopwatch);
	}
}
#endif
