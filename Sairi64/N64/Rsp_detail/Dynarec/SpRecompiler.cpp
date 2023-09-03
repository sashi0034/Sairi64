#include "stdafx.h"
#include "SpRecompiler.h"
#include "N64/N64System.h"

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	void SpRecompileFreshCode(N64System& n64, Rsp& rsp, const SpRecompilingTarget& target)
	{
		asmjit::CodeHolder code{};
		auto&& jit = n64.GetJit();
		code.init(jit.environment());
		x86::Assembler x86Asm(&code);
	}
}
