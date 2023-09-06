#pragma once
#include "N64/N64System.h"
#include "N64/N64Logger.h"

#define OFFSET_TO(type, base, target) (offsetof(type, target) - offsetof(type, base))

namespace N64::Cpu_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	struct AssembleState
	{
		uint32 recompiledLength;
		uint32 scanPc;
		bool scanningDelaySlot;
	};

	struct AssembleContext
	{
		N64System* n64;
		Cpu* cpu;
		x86::Assembler* x86Asm;
		asmjit::Label endLabel;
	};

	enum class DecodedToken
	{
		End,
		Continue,
		Branch,
		BranchLikely,
	};

	static void AssembleStepPc(x86::Assembler& x86Asm, const PcRaw& pc)
	{
		x86Asm.mov(x86::rax, (uint64)&pc.curr); // rax <- *curr
		x86Asm.mov(x86::rcx, x86::qword_ptr(x86::rax, 0)); // rcx <- curr
		x86Asm.mov(x86::qword_ptr(x86::rax, OFFSET_TO(PcRaw, curr, prev)), x86::rcx); // prev <- rcx
		x86Asm.mov(x86::rcx, x86::qword_ptr(x86::rax, OFFSET_TO(PcRaw, curr, next))); // rcx <- next
		x86Asm.mov(x86::qword_ptr(x86::rax, 0), x86::rcx); // curr <- rcx
		x86Asm.add(x86::rcx, 4); // rcx <- rcx+4
		x86Asm.mov(x86::qword_ptr(x86::rax, OFFSET_TO(PcRaw, curr, next)), x86::rcx); // next <- rcx
	}

	static void AssembleStepPc(const AssembleContext& ctx)
	{
		AssembleStepPc(*ctx.x86Asm, ctx.cpu->GetPc().Raw());
	}

	static void AssembleStepDelaySlot(const AssembleContext& ctx)
	{
		// https://learn.microsoft.com/ja-jp/windows-hardware/drivers/debugger/x64-architecture#calling-conventions
		// rcx > ecx > Cx > cl
		auto&& x86Asm = ctx.x86Asm;
		auto&& raw = &ctx.cpu->GetDelaySlot().Raw();
		x86Asm->mov(x86::rax, (uint64)&raw->curr); // rax <- *curr
		x86Asm->movzx(x86::ecx, x86::byte_ptr(x86::rax, 0)); // ecx <- curr
		x86Asm->mov(x86::byte_ptr(x86::rax, OFFSET_TO(DelaySlotRow, curr, prev)), x86::cl); // prev <- cl
		x86Asm->xor_(x86::cl, x86::cl); // cl <- 0
		x86Asm->mov(x86::byte_ptr(x86::rax, 0), x86::cl); // curr <- cl
	}

	template <typename Instr, typename Unit>
	using InterpretOp1 = Unit (*)(Cpu& cpu, Instr instr);

	template <typename Instr, typename Unit>
	using InterpretOp2 = Unit (*)(N64System& n64, Cpu& cpu, Instr instr);

	template <typename Instr, typename Instr1, typename Unit> [[nodiscard]]
	DecodedToken UseInterpreter(const AssembleContext& ctx, Instr instr, InterpretOp1<Instr1, Unit> op)
	{
		static_assert(std::is_convertible<Instr1, Instr>::value);
		N64_TRACE(Cpu, U"use interpreter => " + instr.Stringify());
		auto&& x86Asm = ctx.x86Asm;

		x86Asm->mov(x86::rcx, ctx.cpu);
		x86Asm->mov(x86::rdx, instr.Raw());

		x86Asm->mov(x86::rax, reinterpret_cast<uint64_t>(op));
		x86Asm->call(x86::rax);
		return DecodedToken::End;
	}

	template <typename Instr, typename Instr1, typename Unit> [[nodiscard]]
	static DecodedToken UseInterpreter(const AssembleContext& ctx, Instr instr, InterpretOp2<Instr1, Unit> op)
	{
		static_assert(std::is_convertible<Instr1, Instr>::value);
		N64_TRACE(Cpu, U"use interpreter => " + instr.Stringify());
		auto&& x86Asm = ctx.x86Asm;

		x86Asm->mov(x86::rcx, ctx.n64);
		x86Asm->mov(x86::rdx, ctx.cpu);
		x86Asm->mov(x86::r8d, instr.Raw());

		x86Asm->mov(x86::rax, reinterpret_cast<uint64_t>(op));
		x86Asm->call(x86::rax);
		return DecodedToken::End;
	}

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
			N64_TRACE(Cpu, U"break point! n64={:016X}, code={}"_fmt(reinterpret_cast<uint64>(&n64), code));
		};

		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, ctx.n64);
		x86Asm.mov(x86::rdx, code0);
		x86Asm.mov(x86::rax, func);
		x86Asm.call(x86::rax);
	}
}
