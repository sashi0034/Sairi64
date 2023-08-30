#include <Catch2/catch.hpp>

#include "N64/Cpu_detail/Dynarec/GprMapper.h"
#include "N64/Cpu_detail/Gpr.h"

using namespace N64::Cpu_detail;

// #define SHOW_LOG

TEST_CASE("DynarecTest_GprMapper")
{
	using namespace asmjit;

	JitRuntime jit;
	CodeHolder code;
	code.init(jit.environment());
	x86::Assembler x86Asm(&code);
#ifdef SHOW_LOG
	StringLogger logger{};
	code.setLogger(&logger);
#endif

	static Gpr gpr{};
	for (int i = 0; i < 32; ++i) gpr.Write(i, i * 10);
	Dynarec::GprMapper gprMapper{};
	gprMapper.PushNonVolatiles(x86Asm);
	constexpr int stackSize = 40;
	x86Asm.sub(x86::rsp, stackSize);

	auto gpq_1 = gprMapper.AssignMap(x86Asm, gpr, 1);
	auto gpq_2 = gprMapper.AssignMap(x86Asm, gpr, 2);
	x86Asm.add(gpq_1, 7); // GPR[1] <- 10 + 7
	x86Asm.add(gpq_2, gpq_1); // GPR[2] <- 20 + 17

	for (int i = 28; i < 30; i++)
	{
		gprMapper.AssignMap(x86Asm, gpr, i, i + 1);
		gprMapper.AssignMap(x86Asm, gpr, i, i + 1, i + 2);
	}
	auto [gpq_16, gpq_17] = gprMapper.AssignMap(x86Asm, gpr, 16, 17);
	gpq_1 = gprMapper.AssignMap(x86Asm, gpr, 1);
	x86Asm.add(gpq_16, gpq_1); // GPR[16] <= 160 + 17
	x86Asm.add(gpq_17, gpq_16); // GPR[16] <= 177 + 170

	gprMapper.FlushClear(x86Asm, gpr);
	x86Asm.add(x86::rsp, stackSize);
	gprMapper.PopNonVolatiles(x86Asm);
	x86Asm.ret();

#ifdef SHOW_LOG
	auto codeData = logger.data();
	std::cout << codeData << std::endl;
#endif

	typedef void (*func_t)();
	func_t func;
	const auto error = jit.add(&func, &code);
	REQUIRE(error == 0);
	func();
	jit.release(func);

	REQUIRE(gpr.Read(1) == 17);
	REQUIRE(gpr.Read(2) == 37);
	REQUIRE(gpr.Read(16) == 177);
	REQUIRE(gpr.Read(17) == 347);
}
