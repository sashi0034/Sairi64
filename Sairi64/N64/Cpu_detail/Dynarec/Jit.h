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

			auto&& x86Asm = ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();
			auto [gpqRd, gpqRs, gpqRt] = ctx.gprMapper->AssignMap(*x86Asm, gpr, rd, rs, rt);
			x86Asm->mov(x86::eax, gpqRs.r32());
			x86Asm->add(x86::eax, gpqRt.r32());
			x86Asm->movsxd(gpqRd, x86::eax);
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
