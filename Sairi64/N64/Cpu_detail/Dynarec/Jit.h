﻿#pragma once

#include "JitUtil.h"

#include "../Cpu_Interpreter_Op_Cop.h"

#ifndef DYNAREC_RECOMPILER_INTERNAL
#error "This file is an internal file used by Recompiler"
#endif

#define JIT_ENTRY N64_TRACE(U"JIT => " + instr.Stringify())

namespace N64::Cpu_detail::Dynarec
{
	// x86_64 documents
	// https://learn.microsoft.com/ja-jp/windows-hardware/drivers/debugger/x64-architecture
	// https://www.felixcloutier.com/x86/mov

	using Process = Cpu::Process;

	class Jit
	{
	public:
		template <OpSpecialFunct funct> [[nodiscard]]
		static DecodedToken SPECIAL_arithmetic(const AssembleContext& ctx, InstructionR instr)
		{
			JIT_ENTRY;
			const uint8 rd = instr.Rd();
			if (rd == 0) return DecodedToken::Continue;

			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();

			x86Asm.mov(x86::rax, (uint64)&gpr.Raw()[0]);
			loadGpr(x86Asm, x86::rcx, x86::rax, rs); // rcx <- rs
			loadGpr(x86Asm, x86::rdx, x86::rax, rt); // rdx <- rt

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
			return DecodedToken::Continue;
		}

		template <OpSpecialFunct funct> [[nodiscard]]
		static DecodedToken SPECIAL_shift(const AssembleContext& ctx, InstructionR instr)
		{
			JIT_ENTRY;
			const uint8 rd = instr.Rd();
			if (rd == 0) return DecodedToken::Continue;

			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 sa = instr.Sa();
			const uint8 rt = instr.Rt();
			x86Asm.mov(x86::rax, (uint64)&gpr.Raw()[0]);
			loadGpr(x86Asm, x86::rcx, x86::rax, rt); // rcx <- rt

			if constexpr (funct == OpSpecialFunct::SLL)
			{
				x86Asm.shl(x86::ecx, sa);
				x86Asm.movsxd(x86::rcx, x86::ecx);
			}
			else if constexpr (funct == OpSpecialFunct::SRL)
			{
				x86Asm.shr(x86::ecx, sa);
				x86Asm.movsxd(x86::rcx, x86::ecx);
			}
			else if constexpr (funct == OpSpecialFunct::SRA)
			{
				x86Asm.sar(x86::rcx, sa);
				x86Asm.movsxd(x86::rcx, x86::ecx);
			}
			else if constexpr (funct == OpSpecialFunct::DSLL)
			{
				x86Asm.shl(x86::rcx, sa);
			}
			else if constexpr (funct == OpSpecialFunct::DSRL)
			{
				x86Asm.shr(x86::rcx, sa);
			}
			else if constexpr (funct == OpSpecialFunct::DSRA)
			{
				x86Asm.sar(x86::rcx, sa);
			}
			else if constexpr (funct == OpSpecialFunct::DSLL32)
			{
				x86Asm.shl(x86::rcx, sa + 32);
			}
			else if constexpr (funct == OpSpecialFunct::DSRL32)
			{
				x86Asm.shr(x86::rcx, sa + 32);
			}
			else if constexpr (funct == OpSpecialFunct::DSRA32)
			{
				x86Asm.sar(x86::rcx, sa + 32);
			}
			else
			{
				static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);
			}

			x86Asm.mov(qword_ptr(x86::rax, rd * 8), x86::rcx);
			return DecodedToken::Continue;
		}

		[[nodiscard]]
		static DecodedToken CACHE(InstructionR instr)
		{
			JIT_ENTRY;
			return DecodedToken::Continue;
		}

		template <Opcode op> [[nodiscard]]
		static DecodedToken B_branch(const AssembleContext& ctx, InstructionI instr)
		{
			JIT_ENTRY;
			constexpr BranchType branchType =
				op == Opcode::BEQL ||
				op == Opcode::BNEL
					? BranchType::Likely
					: BranchType::Normal;
			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			auto&& pc = ctx.cpu->GetPc().Raw();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();
			sint64 offset = static_cast<sint16>(instr.Imm());
			offset *= 4; // left shift 2

			x86Asm.mov(x86::rax, (uint64)&gpr.Raw()[0]);
			loadGpr(x86Asm, x86::rcx, x86::rax, rs); // rcx <- rs
			loadGpr(x86Asm, x86::rdx, x86::rax, rt); // rdx <- rt
			if constexpr (op == Opcode::BEQ || op == Opcode::BEQL)
			{
				x86Asm.cmp(x86::rcx, x86::rdx);
				x86Asm.sete(x86::r8b); // r8b <- rs==rt
			}
			else if constexpr (op == Opcode::BNE || op == Opcode::BNEL)
			{
				x86Asm.cmp(x86::rcx, x86::rdx);
				x86Asm.setne(x86::r8b); // r8b <- rs!=rt
			}
			else
			{
				static_assert(Utils::AlwaysFalseValue<Opcode, op>);
			}
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.mov(x86::rdx, x86::rax); // rdx <- pc.curr
			x86Asm.mov(x86::rax, offset); // rax <- immediate
			x86Asm.add(x86::rdx, x86::rax); // rdx <- pc.curr + rax
			x86Asm.mov(x86::rcx, (uint64)ctx.cpu); // rcx <- *cpu
			x86Asm.mov(x86::rax, reinterpret_cast<uint64>(&Process::BranchVAddr64<branchType>));
			x86Asm.call(x86::rax);

			return branchType == BranchType::Normal ? DecodedToken::Branch : DecodedToken::BranchLikely;
		}

		template <Opcode op> [[nodiscard]]
		static DecodedToken L_load(const AssembleContext& ctx, const AssembleState& state, InstructionI instr)
		{
			JIT_ENTRY;
			auto&& x86Asm = *ctx.x86Asm;
			auto&& gpr = ctx.cpu->GetGpr();
			const uint8 rs = instr.Rs();
			const uint8 rt = instr.Rt();
			const sint64 offset = static_cast<sint16>(instr.Imm());
			const auto resolvedLabel = x86Asm.newLabel();

			// TODO: check address error?

			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs])));
			x86Asm.mov(x86::rdx, x86::rax); // rdx <- rs
			x86Asm.add(x86::rdx, offset); // rdx <- rs + offset
			x86Asm.mov(x86::rcx, (uint64)ctx.cpu); // rcx <- *cpu
			x86Asm.mov(x86::rax, (uint64)&callResolveVAddr);
			x86Asm.call(x86::rax); // eax <- paddr
			x86Asm.cmp(x86::eax, static_cast<uint32>(ResolvedPAddr32::InvalidAddress)); // if eax is not invalid
			x86Asm.jne(resolvedLabel); // then goto @resolved
			// now, paddr is invalid
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs])));
			x86Asm.mov(x86::rdx, x86::rax); // rdx <- rs
			x86Asm.add(x86::rdx, offset); // rdx <- rs + offset
			x86Asm.mov(x86::rcx, (uint64)ctx.cpu); // rcx <- *cpu
			x86Asm.mov(x86::rax, (uint64)&handleResolvingError<BusAccess::Load, 0>);
			x86Asm.call(x86::rax); // handling
			x86Asm.mov(x86::rax, state.recompiledLength);
			x86Asm.jmp(ctx.endLabel); // goto @end

			x86Asm.bind(resolvedLabel); // @resolved
			if (rt == 0) return DecodedToken::Continue;
			x86Asm.mov(x86::rdx, x86::rax); // rdx <- paddr
			x86Asm.mov(x86::rcx, (uint64)ctx.n64); // rcx <- *n64
			if constexpr (op == Opcode::LB || op == Opcode::LBU)
			{
				x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr8);
				x86Asm.call(x86::rax); // eax <- value
				if constexpr (op == Opcode::LB) x86Asm.movsxd(x86::rax, x86::al); // rax <- sign-extended al
			}
			else if constexpr (op == Opcode::LH || op == Opcode::LHU)
			{
				x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr16);
				x86Asm.call(x86::rax); // eax <- value
				if constexpr (op == Opcode::LH) x86Asm.movsxd(x86::rax, x86::ax); // rax <- sign-extended ax
			}
			else if constexpr (op == Opcode::LW || op == Opcode::LWU)
			{
				x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr32);
				x86Asm.call(x86::rax); // rax <- value
				if constexpr (op == Opcode::LW) x86Asm.movsxd(x86::rax, x86::eax); // rax <- sign-extended eax
			}
			else if constexpr (op == Opcode::LD)
			{
				x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr64);
				x86Asm.call(x86::rax); // eax <- value
			}
			else
			{
				static_assert(AlwaysFalseValue<Opcode, op>);
			}
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rt])), x86::rax); // rt <- rax
			return DecodedToken::Continue;
		}

	private:
		static void loadGpr(
			x86::Assembler& x86Asm,
			const x86::Gpq& dest,
			const x86::Gpq& base,
			const uint8 gprIndex)
		{
			if (gprIndex == 0)
				x86Asm.xor_(dest, dest); // gp <- 0
			else
				x86Asm.mov(dest, x86::qword_ptr(base, gprIndex * 8)); // gp <- gpr[index]
		}

		static uint32 callResolveVAddr(Cpu* cpu, uint64 vaddr)
		{
			// to disable hidden pointer return value
			return Mmu::ResolveVAddr(*cpu, vaddr).value();
		}

		template <BusAccess access, int cop>
		static void handleResolvingError(Cpu& cpu, uint64 vaddr)
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			Process::ThrowException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
		}
	};
}
