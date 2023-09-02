#pragma once
#include "Jit.h"

class N64::Cpu_detail::Dynarec::Jit::Cop
{
public:
	template <OpCopSub sub> [[nodiscard]]
	static DecodedToken MFC0_template(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;
		const uint8 rd = instr.Rd();
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)&ctx.cpu->GetCop0());
		x86Asm.mov(x86::rdx, rd);
		if constexpr (sub == OpCopSub::MFC)
		{
			x86Asm.mov(x86::rax, (uint64)&cop0Read32);
			x86Asm.call(x86::rax);
			x86Asm.movsxd(x86::rax, x86::eax);
		}
		else if constexpr (sub == OpCopSub::DMFC)
		{
			x86Asm.mov(x86::rax, (uint64)&cop0Read64);
			x86Asm.call(x86::rax);
		}
		else static_assert(AlwaysFalseValue<OpCopSub, sub>);
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rt])), x86::rax);
		return DecodedToken::Continue;
	}

private:
	static uint32 cop0Read32(const Cop0& cop0, uint8 reg)
	{
		return cop0.Read32(reg);
	}

	static uint64 cop0Read64(const Cop0& cop0, uint8 reg)
	{
		return cop0.Read64(reg);
	}
};
