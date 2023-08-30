#include "stdafx.h"
#include "Recompiler_x86_64.h"

#include "N64/N64System.h"

namespace N64::Cpu_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	void assembleCode(N64System& n64, Cpu& cpu, PAddr32 startPc, x86::Assembler& x86Asm)
	{
	}

	RecompiledResult RecompileFreshCode(N64System& n64, Cpu& cpu, PAddr32 startPc)
	{
		RecompiledResult result{};
		asmjit::CodeHolder code{};
		auto&& jit = n64.GetJit();
		code.init(jit.environment());
		x86::Assembler x86Asm(&code);

		assembleCode(n64, cpu, startPc, x86Asm);

		jit.add(&result.code, &code);
		return result;
	}
}
