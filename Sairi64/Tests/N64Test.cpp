#include "stdafx.h"

#include <Catch2/catch.hpp>


#include "N64/Mmu.h"
#include "N64/N64Frame.h"
#include "N64/N64Singleton.h"
#include "Utils/Util.h"

#if 1
namespace Tests
{
	bool Dillon64Test(const String& filePath)
	{
		using namespace N64;

		const auto n64 = std::make_unique<N64Singleton>();
		N64System& n64System = n64->GetSystem();
		N64Frame n64Frame{};

		// テスト用の初期化
		n64Frame.Init(n64System, {
			              .filePath = filePath,
			              .executePifRom = false
		              });
		for (uint32 i = 0; i < 0x100000; i += 4)
		{
			const uint32 data = Mmu::ReadPaddr32(n64System, PAddr32(0x10001000 + i));
			Mmu::WritePaddr32(n64System, PAddr32(0x00001000 + i), data);
		}
		n64System.GetCpu().GetPc().Change64(0x80001000);

		// N64コンソール実行
		constexpr int targetR30 = 30;
		n64Frame.RunOnConsole(n64System, [&]()
		{
			return n64System.GetCpu().GetGpr().Read(targetR30) != 0;
		});

		// キー入力待機
		Utils::WaitAnyKeyOnConsole();

		const int expected_minus1 = n64System.GetCpu().GetGpr().Read(targetR30);
		return expected_minus1 == -1;
	}

	TEST_CASE("N64Test basic_simpleboot")
	{
		// REQUIRE(Dillon64Test(U"asset/rom/dillonb-n64-tests/basic_simpleboot.z64"));
	}

	TEST_CASE("N64Test addiu_simpleboot")
	{
		REQUIRE(Dillon64Test(U"asset/rom/dillonb-n64-tests/addiu_simpleboot.z64"));
	}
}
#endif
