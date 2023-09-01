#include <Catch2/catch.hpp>
#include "N64/Cpu_detail/Dynarec/JitUtil.h"

using namespace N64::Cpu_detail;

// #define SHOW_LOG

TEST_CASE("DynarecTest_AssembleStepPc")
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

	PcRaw pc{0x10, 0x14, 0x18};
	Dynarec::AssembleStepPc(x86Asm, pc);
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
	REQUIRE(pc.prev == 0x14);
	REQUIRE(pc.curr == 0x18);
	REQUIRE(pc.next == 0x1C);

	func();
	REQUIRE(pc.prev == 0x18);
	REQUIRE(pc.curr == 0x1C);
	REQUIRE(pc.next == 0x20);

	jit.release(func);
}
