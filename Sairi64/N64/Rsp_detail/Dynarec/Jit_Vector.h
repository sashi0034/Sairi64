#pragma once

#include "Jit.h"

class N64::Rsp_detail::Dynarec::Jit::Vector
{
public:
	static DecodedToken CTC2(const AssembleContext& ctx, InstructionCop2VecSub instr)
	{
		JIT_SP;

		return DecodedToken::Continue;
	}

private:
};
