#pragma once

#include "JitForward.h"
#include "N64/Instruction.h"
#ifndef RSP_PROCESS_INTERNAL
#error "This file is an internal file used by SpRecompiler"
#endif

#define JIT_SP N64_TRACE(U"JIT-SP => " + instr.Stringify())

namespace N64::Rsp_detail::Dynarec
{
	class Jit;
}

class N64::Rsp_detail::Dynarec::Jit
{
public:
	[[nodiscard]]
	static DecodedToken LUI(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;
		const sint32 imm = (uint16)instr.Imm() << 16;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&ctx.rsp->GetGpr().Raw()[rt])), imm);
		return DecodedToken::Continue;
	}

private:
};
