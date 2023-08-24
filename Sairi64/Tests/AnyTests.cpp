#include "stdafx.h"
#include <Catch2/catch.hpp>

#include "N64/Scheduler.h"
#include "N64/Cpu_detail/Instruction.h"

namespace Tests
{
	TEST_CASE("N64Test_Scheduler")
	{
		N64::Scheduler scheduler{10};
		int test = 0;

		scheduler.ScheduleEvent(20, [&test]() { test *= 5; });
		scheduler.ScheduleEvent(10, [&test]() { test = 1; });
		scheduler.ScheduleEvent(30, [&test]() { test += 10; });

		for (int i = 0; i < 100; ++i) scheduler.Step();
		REQUIRE(test == 15);

		scheduler.ScheduleEvent(50, [&test]() { test *= 10; });
		scheduler.ScheduleEvent(0, [&test]() { test *= 2; });
		scheduler.ScheduleEvent(40, [&test]() { test -= 20; });

		for (int i = 0; i < 100; ++i) scheduler.Step();
		REQUIRE(test == 100);
	}

	TEST_CASE("N64Test_Instruction")
	{
		using namespace N64::Cpu_detail;
		const InstructionI instr1 = {0x3C'04'BF'C0};
		const sint32 instr1_imm = static_cast<sint16>(instr1.Imm());
		REQUIRE(instr1_imm == 0xFF'FF'BF'C0);
	}
}
