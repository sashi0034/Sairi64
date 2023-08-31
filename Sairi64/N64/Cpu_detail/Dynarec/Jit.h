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
		template <OpSpecialFunct funct> [[nodiscard]]
		static DecodeNext SPECIAL_templateArithmetic(const AssembleContext& ctx, InstructionR instr)
		{
			JIT_ENTRY;
			const uint8 rd = instr.Rd();
			if (rd == 0) return DecodeNext::Continue;

			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();

			x86Asm.mov(x86::rax, (uint64)&gpr.Raw()[0]);

			if (rs == 0)
				x86Asm.xor_(x86::rcx, x86::rcx); // rcx <- 0
			else
				x86Asm.mov(x86::rcx, x86::qword_ptr(x86::rax, rs * 8)); // rcx <- gpr[rs]

			if (rt == 0)
				x86Asm.xor_(x86::rdx, x86::rdx); // rdx <- 0
			else
				x86Asm.mov(x86::rdx, x86::qword_ptr(x86::rax, rt * 8)); // rdx <- gpr[rt]

			if constexpr (funct == OpSpecialFunct::ADDU)
			{
				x86Asm.add(x86::ecx, x86::edx);
				x86Asm.movsxd(x86::rcx, x86::ecx);
			}
			else if constexpr (funct == OpSpecialFunct::DADDU)
			{
				x86Asm.add(x86::rcx, x86::rdx);
			}
			else if constexpr (funct == OpSpecialFunct::SUBU)
			{
				x86Asm.sub(x86::ecx, x86::edx);
				x86Asm.movsxd(x86::rcx, x86::ecx);
			}
			else if constexpr (funct == OpSpecialFunct::DSUBU)
			{
				x86Asm.sub(x86::ecx, x86::edx);
			}
			else if constexpr (funct == OpSpecialFunct::AND)
			{
				x86Asm.and_(x86::rcx, x86::rdx);
			}
			else if constexpr (funct == OpSpecialFunct::OR)
			{
				x86Asm.or_(x86::rcx, x86::rdx);
			}
			else if constexpr (funct == OpSpecialFunct::XOR)
			{
				x86Asm.xor_(x86::rcx, x86::rdx);
			}
			else if constexpr (funct == OpSpecialFunct::NOR)
			{
				x86Asm.or_(x86::rcx, x86::rdx);
				x86Asm.not_(x86::rcx);
			}
			else
			{
				static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);
			}

			x86Asm.mov(x86::qword_ptr(x86::rax, rd * 8), x86::rcx); // gpr[rd] <- rcx
			return DecodeNext::Continue;
		}

		[[nodiscard]]
		static DecodeNext CACHE(InstructionR instr)
		{
			JIT_ENTRY;
			return DecodeNext::Continue;
		}

	private:
	};
}
