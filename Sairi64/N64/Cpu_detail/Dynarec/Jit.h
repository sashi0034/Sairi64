#pragma once

#include "JitUtil.h"

#include "../Cpu_Interpreter_Op_Cop.h"

#ifndef DYNAREC_RECOMPILER_INTERNAL
#error "This file is an internal file used by Recompiler"
#endif

#define JIT_ENTRY N64_TRACE(U"JIT => " + instr.Stringify())

namespace N64::Cpu_detail::Dynarec
{
	class Jit
	{
	public:
		[[nodiscard]]
		static EndFlag ADDU(const AssembleContext& ctx, InstructionR instr)
		{
			JIT_ENTRY;
			const uint8 rd = instr.Rd();
			if (rd == 0) return false;

			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();

			x86Asm.mov(x86::rax, (uint64)&gpr.Raw()[0]);
			x86Asm.mov(x86::rcx, x86::qword_ptr(x86::rax, rs * 8)); // rcx <- gpr[rs]
			x86Asm.mov(x86::rdx, x86::qword_ptr(x86::rax, rt * 8)); // rcx <- gpr[rt]
			x86Asm.add(x86::ecx, x86::edx);
			x86Asm.movsxd(x86::r8, x86::ecx);
			x86Asm.mov(x86::qword_ptr(x86::rax, rd * 8), x86::r8); // ecx <- gpr[rt]
			return false;
		}

		[[nodiscard]]
		static EndFlag CACHE(InstructionR instr)
		{
			JIT_ENTRY;
			return false;
		}

	private:
	};
}
