#pragma once
#include "N64/N64Logger.h"
#include "../Rsp_Process.h"

#define OFFSET_TO(type, base, target) (offsetof(type, target) - offsetof(type, base))

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	using Process = Rsp::Process;

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

	static void CallBreakPoint(const AssembleContext& ctx, uint64 code0 = 0)
	{
		static void (*func)(N64System& n64, uint64 code) = [](N64System& n64, uint64 code)
		{
			N64_TRACE(U"break point! n64={:016X}, code={}"_fmt(reinterpret_cast<uint64>(&n64), code));
		};

		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, ctx.n64);
		x86Asm.mov(x86::rdx, code0);
		x86Asm.mov(x86::rax, func);
		x86Asm.call(x86::rax);
	}
}
