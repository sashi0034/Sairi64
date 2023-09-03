#pragma once
#include "N64/N64Logger.h"

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	struct AssembleState
	{
		uint16 recompiledLength;
		uint16 scanPc;
	};

	struct AssembleContext
	{
		N64System* n64;
		Rsp* rsp;
		x86::Assembler* x86Asm;
		asmjit::Label endLabel;
	};

	enum class DecodedToken
	{
		End,
		Continue,
		Branch,
	};

	template <typename Instr> [[nodiscard]]
	static DecodedToken AssumeNotImplemented(const AssembleContext& ctx, Instr instr)
	{
		static void (*func)(Instr) = [](Instr instruction)
		{
			N64Logger::Abort(U"not implemented: instruction {}"_fmt(instruction.Stringify()));
		};

		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, instr.Raw());
		x86Asm.mov(x86::rax, func);
		x86Asm.call(x86::rax);
		return DecodedToken::End;
	}
}
