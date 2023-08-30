#pragma once
#include "GprMapper.h"
#include "N64/N64System.h"

#ifndef DYNAREC_RECOMPILER_INTERNAL
#error "This file is an internal file used by Recompiler"
#endif

#define OFFSET_TO(type, base, target) (offsetof(type, target) - offsetof(type, base))

namespace N64::Cpu_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	struct AssembleState
	{
		uint32 recompiledLength;
		uint32 scanPc;
		Pc shadowScanPc;
		DelaySlot scanDelaySlot;
	};

	struct AssembleContext
	{
		N64System* n64;
		Cpu* cpu;
		GprMapper* gprMapper;
		x86::Assembler* x86Asm;
	};

	using EndFlag = bool;

	static void FlashPc(const AssembleContext& ctx,
	                    uint64 prevPc, uint64 currPc, uint64 nextPc)
	{
		auto&& x86Asm = ctx.x86Asm;
		auto&& raw = &ctx.cpu->GetPc().Raw();
		x86Asm->mov(x86::rax, (uint64)&raw->curr);
		x86Asm->mov(x86::qword_ptr(x86::rax, OFFSET_TO(PcRaw, curr, prev)), prevPc);
		x86Asm->mov(x86::qword_ptr(x86::rax, 0), currPc);
		x86Asm->mov(x86::qword_ptr(x86::rax, OFFSET_TO(PcRaw, curr, next)), nextPc);
	}

	static void FlashPc(const AssembleContext& ctx, const Pc& pc)
	{
		return FlashPc(ctx, pc.Prev(), pc.Curr(), pc.Next());
	}

	static void FlashDelaySlot(const AssembleContext& ctx, const DelaySlot& delaySlot)
	{
		auto&& x86Asm = ctx.x86Asm;
		x86Asm->mov(x86::rax, (uint64)&ctx.cpu->GetDelaySlot().Raw().curr);
		x86Asm->mov(x86::qword_ptr(x86::rax, OFFSET_TO(DelaySlotRow, curr, prev)), delaySlot.Prev());
		x86Asm->mov(x86::qword_ptr(x86::rax, 0), delaySlot.Curr());
	}

	template <typename Instr, typename Unit>
	using InterpretOp1 = Unit (*)(Cpu& cpu, Instr instr);

	template <typename Instr, typename Unit>
	using InterpretOp2 = Unit (*)(N64System& n64, Cpu& cpu, Instr instr);

	template <typename Instr, typename Instr1, typename Unit> [[nodiscard]]
	EndFlag UseInterpreter(const AssembleContext& ctx, Instr instr, InterpretOp1<Instr1, Unit> op)
	{
		static_assert(std::is_convertible<Instr1, Instr>::value);
		N64_TRACE(U"use interpreter => " + instr.Stringify());
		auto&& x86Asm = ctx.x86Asm;
		ctx.gprMapper->FlushClear(*x86Asm, ctx.cpu->GetGpr());

		x86Asm->mov(x86::rcx, ctx.cpu);
		x86Asm->mov(x86::rdx, instr.Raw());

		x86Asm->mov(x86::rax, reinterpret_cast<uint64_t>(op));
		x86Asm->call(x86::rax);
		return true;
	}

	template <typename Instr, typename Instr1, typename Unit> [[nodiscard]]
	static EndFlag UseInterpreter(const AssembleContext& ctx, Instr instr, InterpretOp2<Instr1, Unit> op)
	{
		static_assert(std::is_convertible<Instr1, Instr>::value);
		N64_TRACE(U"use interpreter => " + instr.Stringify());
		auto&& x86Asm = ctx.x86Asm;
		ctx.gprMapper->FlushClear(*x86Asm, ctx.cpu->GetGpr());

		x86Asm->mov(x86::rcx, ctx.n64);
		x86Asm->mov(x86::rdx, ctx.cpu);
		x86Asm->mov(x86::r8d, instr.Raw());

		x86Asm->mov(x86::rax, reinterpret_cast<uint64_t>(op));
		x86Asm->call(x86::rax);
		return true;
	}
}
