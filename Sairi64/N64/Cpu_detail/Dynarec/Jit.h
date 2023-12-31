﻿#pragma once

#include "JitUtil.h"
#include "N64/Cpu_detail/Cpu_Process.h"

#ifndef DYNAREC_RECOMPILER_INTERNAL
#error "This file is an internal file used by Recompiler"
#endif

#define JIT_ENTRY N64_TRACE(Cpu, U"JIT => " + instr.Stringify())

namespace N64::Cpu_detail::Dynarec
{
	// x86_64 documents
	// https://learn.microsoft.com/ja-jp/windows-hardware/drivers/debugger/x64-architecture
	// https://www.felixcloutier.com/x86/mov

	class Jit;

	using Process = Cpu::Process;
}

class N64::Cpu_detail::Dynarec::Jit
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

	template <Opcode op> [[nodiscard]]
	static DecodedToken I_immediateArithmetic(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const uint8 rs = instr.Rs();
		const uint16 imm = instr.Imm();

		if (rs == 0)
			x86Asm.xor_(x86::rax, x86::rax); // rax <- 0
		else
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs]))); // rax <- rs

		if constexpr (op == Opcode::ADDIU)
		{
			x86Asm.add(x86::eax, (sint32)static_cast<sint16>(imm)); // eax <- eax + immediate
			x86Asm.movsxd(x86::rax, x86::eax); // sign-extend
		}
		else if constexpr (op == Opcode::DADDIU)
		{
			x86Asm.add(x86::rax, (sint64)static_cast<sint16>(imm));
		}
		else if constexpr (op == Opcode::ANDI)
		{
			x86Asm.and_(x86::rax, (uint64)imm);
		}
		else if constexpr (op == Opcode::ORI)
		{
			x86Asm.or_(x86::rax, (uint64)imm);
		}
		else if constexpr (op == Opcode::XORI)
		{
			x86Asm.xor_(x86::rax, (uint64)imm);
		}
		else
		{
			static_assert(AlwaysFalseValue<Opcode, op>);
		}

		x86Asm.mov(x86::rcx, (uint64)&gpr.Raw()[rt]); // rcx <- *rt
		x86Asm.mov(x86::qword_ptr(x86::rcx), x86::rax); // rcx <- rax
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken MULT_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& cpu = *ctx.cpu;
		auto&& gpr = cpu.GetGpr().Raw();
		const uint8 rs = instr.Rs();
		const uint8 rt = instr.Rt();

		if constexpr (funct == OpSpecialFunct::MULT)
		{
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rs])));
			x86Asm.movsxd(x86::rcx, x86::eax);
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rt])));
			x86Asm.movsxd(x86::rdx, x86::eax);
		}
		else if constexpr (funct == OpSpecialFunct::MULTU)
		{
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rs])));
			x86Asm.mov(x86::ecx, x86::eax);
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rt])));
			x86Asm.mov(x86::edx, x86::eax);
		}
		else static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
		x86Asm.imul(x86::rcx, x86::rdx);
		x86Asm.movsxd(x86::rax, x86::ecx); // rax <- lo 32-bits sign-extended
		if constexpr (funct == OpSpecialFunct::MULT)
			x86Asm.sar(x86::rcx, 32); // rcx <- hi 32-bits
		else if constexpr (funct == OpSpecialFunct::MULTU)
			x86Asm.shr(x86::rcx, 32); // rcx <- hi 32-bits
		else static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressLo(cpu))), x86::rax);
		x86Asm.movsxd(x86::rax, x86::ecx); // rax <- hi 32-bits sign-extended
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressHi(cpu))), x86::rax);
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken D_multiplyDivide(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& cpu = *ctx.cpu;
		auto&& gpr = cpu.GetGpr().Raw();
		const uint8 rs = instr.Rs();
		const uint8 rt = instr.Rt();
		x86Asm.mov(x86::rcx, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rs])));
		x86Asm.mov(x86::rdx, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rt])));
		x86Asm.mov(x86::r8, Process::AddressLo(cpu));
		x86Asm.mov(x86::r9, Process::AddressHi(cpu));
		if constexpr (funct == OpSpecialFunct::DMULT)
			x86Asm.call((uint64)&helperDMULT);
		else if constexpr (funct == OpSpecialFunct::DMULTU)
			x86Asm.call((uint64)&helperDMULTU);
		else if constexpr (funct == OpSpecialFunct::DDIV)
			x86Asm.call((uint64)&helperDDIV);
		else if constexpr (funct == OpSpecialFunct::DDIVU)
			x86Asm.call((uint64)&helperDDIVU);
		else static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken MF_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& cpu = *ctx.cpu;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;
		if constexpr (funct == OpSpecialFunct::MFLO)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressLo(cpu))));
		else if constexpr (funct == OpSpecialFunct::MFHI)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressHi(cpu))));
		else static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&cpu.GetGpr().Raw()[rd])), x86::rax);
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken MT_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& cpu = *ctx.cpu;
		const uint8 rs = instr.Rs();
		if (rs != 0)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&cpu.GetGpr().Raw()[rs])));
		else
			x86Asm.xor_(x86::rax, x86::rax);
		if constexpr (funct == OpSpecialFunct::MTLO)
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressLo(cpu))), x86::rax);
		else if constexpr (funct == OpSpecialFunct::MTHI)
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(Process::AddressHi(cpu))), x86::rax);
		else static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
		return DecodedToken::Continue;
	}

	[[nodiscard]]
	static DecodedToken LUI(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;
		sint64 imm = static_cast<sint16>(instr.Imm());
		imm *= 0x10000;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rt])), imm);
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SLT_template(const AssembleContext& ctx, InstructionR instr)
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

		x86Asm.xor_(x86::r8, x86::r8);
		x86Asm.cmp(x86::rcx, x86::rdx);
		if constexpr (funct == OpSpecialFunct::SLT)
			x86Asm.setl(x86::r8b);
		else if constexpr (funct == OpSpecialFunct::SLTU)
			x86Asm.setb(x86::r8b);
		else
			static_assert(AlwaysFalseValue<Opcode, funct>);
		x86Asm.mov(x86::qword_ptr(x86::rax, rd * 8), x86::r8); // gpr[rd] <- rcx
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken SLTI_template(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_ENTRY;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const uint8 rs = instr.Rs();
		const uint16 imm = instr.Imm();

		if (rs == 0)
			x86Asm.xor_(x86::rax, x86::rax); // rax <- 0
		else
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs]))); // rax <- rs
		x86Asm.xor_(x86::rdx, x86::rdx);
		x86Asm.cmp(x86::rax, (sint64)static_cast<sint16>(imm));
		if constexpr (op == Opcode::SLTI)
			x86Asm.setl(x86::dl);
		else if constexpr (op == Opcode::SLTIU)
			x86Asm.setb(x86::dl);
		else
			static_assert(AlwaysFalseValue<Opcode, op>);
		x86Asm.mov(x86::rcx, (uint64)&gpr.Raw()[rt]); // rcx <- *rt
		x86Asm.mov(x86::qword_ptr(x86::rcx), x86::rdx); // rcx <- rax
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

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SPECIAL_shiftVariable(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;

		constexpr bool dword = []() consteval
		{
			return
				funct == OpSpecialFunct::DSLLV ||
				funct == OpSpecialFunct::DSRLV ||
				funct == OpSpecialFunct::DSRAV;
		}();
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr().Raw();
		const uint8 rt = instr.Rt();
		const uint8 rs = instr.Rs();

		if (rt != 0)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rt]))); // rax <- rt
		else
			x86Asm.xor_(x86::rax, x86::rax);
		if (rs != 0)
		{
			x86Asm.mov(x86::rcx, x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rs]))); // rcx <- rs
			x86Asm.and_(x86::rcx, dword ? 0b111111 : 0b11111);
		}
		else
			x86Asm.xor_(x86::rcx, x86::rcx);

		if constexpr (funct == OpSpecialFunct::SLLV)
		{
			x86Asm.shl(x86::eax, x86::cl);
			x86Asm.movsxd(x86::rax, x86::eax);
		}
		else if constexpr (funct == OpSpecialFunct::SRLV)
		{
			x86Asm.shr(x86::eax, x86::cl);
			x86Asm.movsxd(x86::rax, x86::eax);
		}
		else if constexpr (funct == OpSpecialFunct::SRAV)
		{
			x86Asm.sar(x86::rax, x86::cl);
			x86Asm.movsxd(x86::rax, x86::eax);
		}
		else if constexpr (funct == OpSpecialFunct::DSLLV)
		{
			x86Asm.shl(x86::rax, x86::cl);
		}
		else if constexpr (funct == OpSpecialFunct::DSRLV)
		{
			x86Asm.shr(x86::rax, x86::cl);
		}
		else if constexpr (funct == OpSpecialFunct::DSRAV)
		{
			x86Asm.sar(x86::rax, x86::cl);
		}
		else static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);

		x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&gpr[rd])), x86::rax); // gpr[rd] <- rax
		return DecodedToken::Continue;
	}

	[[nodiscard]]
	static DecodedToken CACHE(InstructionR instr)
	{
		JIT_ENTRY;
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken B_branchOffset(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_ENTRY;
		return branchOffsetInternal<op, OpRegimm::Invalid_0xFF>(ctx, instr);
	}

	template <OpRegimm sub> [[nodiscard]]
	static DecodedToken B_branchOffset(const AssembleContext& ctx, InstructionRegimm instr)
	{
		JIT_ENTRY;
		return branchOffsetInternal<Opcode::Invalid_0xFF, sub>(ctx, instr);
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken J_template(const AssembleContext& ctx, InstructionJ instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& pc = ctx.cpu->GetPc().Raw();
		const uint64 target = static_cast<uint64>(instr.Target()) << 2;

		if constexpr (op == Opcode::J)
		{
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.sub(x86::rax, 4);
		}
		else if constexpr (op == Opcode::JAL)
		{
			// link register
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.add(x86::rax, 4);
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[GprRA_31])), x86::rax);
			x86Asm.sub(x86::rax, 8);
		}
		else
		{
			static_assert(AlwaysFalseValue<Opcode, op>);
		}
		// rax <- pc.curr - 4
		x86Asm.and_(x86::rax, 0xFFFFFFFF'F0000000);
		x86Asm.or_(x86::rax, target);
		x86Asm.mov(x86::rdx, x86::rax);
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.call((uint64)&Process::StaticBranchVAddr64<BranchType::Normal, true>);
		return DecodedToken::Branch;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken JR_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		const uint8 rs = instr.Rs();
		x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rs])));
		x86Asm.mov(x86::rdx, x86::rax);
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu);
		x86Asm.mov(x86::rax, &Process::StaticBranchVAddr64<BranchType::Normal, true>);
		x86Asm.call(x86::rax);
		if constexpr (funct == OpSpecialFunct::JR)
		{
			return DecodedToken::Branch;
		}
		else if constexpr (funct == OpSpecialFunct::JALR)
		{
			// link register
			const uint8 rd = instr.Rd();
			if (rd == 0) return DecodedToken::Branch;
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetPc().Raw().curr)));
			x86Asm.add(x86::rax, 4);
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[rd])), x86::rax);
			return DecodedToken::Branch;
		}
		else
		{
			static_assert(AlwaysFalseValue<OpSpecialFunct, funct>);
			return {};
		}
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken L_load(const AssembleContext& ctx, const AssembleState& state, InstructionI instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const uint8 rt = instr.Rt();

		// TODO: check address error?

		preprocessMemoryAccess<BusAccess::Load>(ctx, state, instr);
		// now, paddr is stored in rax

		x86Asm.mov(x86::rdx, x86::rax); // rdx <- paddr
		x86Asm.mov(x86::rcx, (uint64)ctx.n64); // rcx <- *n64
		if constexpr (op == Opcode::LB || op == Opcode::LBU)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr8);
			x86Asm.call(x86::rax); // eax <- value
			if constexpr (op == Opcode::LB)
				x86Asm.movsx(x86::rax, x86::al); // rax <- sign-extended al
			else
				x86Asm.movzx(x86::eax, x86::al); // rax <- zero-extended al
		}
		else if constexpr (op == Opcode::LH || op == Opcode::LHU)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr16);
			x86Asm.call(x86::rax); // eax <- value
			if constexpr (op == Opcode::LH)
				x86Asm.movsx(x86::rax, x86::ax); // rax <- sign-extended ax
			else
				x86Asm.movzx(x86::eax, x86::ax); // rax <- zero-extended ax
		}
		else if constexpr (op == Opcode::LW || op == Opcode::LWU)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::ReadPaddr32);
			x86Asm.call(x86::rax); // rax <- value
			if constexpr (op == Opcode::LW)
				x86Asm.movsxd(x86::rax, x86::eax); // rax <- sign-extended eax
			else
				x86Asm.movzx(x86::eax, x86::eax); // rax <- zero-extended eax
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
		if (rt != 0) x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rt])), x86::rax); // rt <- rax
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken S_store(const AssembleContext& ctx, const AssembleState& state, InstructionI instr)
	{
		JIT_ENTRY;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const uint8 rt = instr.Rt();

		// TODO: check address error?

		preprocessMemoryAccess<BusAccess::Store>(ctx, state, instr);
		// now, paddr is stored in rax

		x86Asm.mov(x86::rdx, x86::rax); // rdx <- paddr
		x86Asm.mov(x86::rcx, (uint64)ctx.n64); // rcx <- *n64
		x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rt]))); // rax <- rt
		x86Asm.mov(x86::r8, x86::rax);

		if constexpr (op == Opcode::SB)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::WritePaddr8);
		}
		else if constexpr (op == Opcode::SH)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::WritePaddr16);
		}
		else if constexpr (op == Opcode::SW)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::WritePaddr32);
		}
		else if constexpr (op == Opcode::SD)
		{
			x86Asm.mov(x86::rax, (uint64)&Mmu::WritePaddr64);
		}
		else
		{
			static_assert(AlwaysFalseValue<Opcode, op>);
		}
		x86Asm.call(x86::rax); // write rt
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken L_loadShifted(const AssembleContext& ctx, const AssembleState& state, InstructionFi instr)
	{
		JIT_ENTRY;
		return memoryAccessShifted<op, BusAccess::Load>(ctx, state, instr);
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken S_storeShifted(const AssembleContext& ctx, const AssembleState& state, InstructionFi instr)
	{
		JIT_ENTRY;
		return memoryAccessShifted<op, BusAccess::Store>(ctx, state, instr);
	}

	class Cop;

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

	N64_ABI static uint32 callResolveVAddr(Cpu* cpu, uint64 vaddr)
	{
		// to disable hidden pointer return value
		return Mmu::ResolveVAddr(*cpu, vaddr).value();
	}

	template <BusAccess access, int cop>
	N64_ABI static void handleResolvingError(Cpu& cpu, uint64 vaddr)
	{
		cpu.GetCop0().HandleTlbException(vaddr);
		Process::ThrowException(cpu, cpu.GetCop0().GetTlbExceptionCode<access>(), cop);
	}

	template <BusAccess access>
	static void preprocessMemoryAccess(
		const AssembleContext& ctx, const AssembleState& state, InstructionI instr)
	{
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const uint8 rs = instr.Rs();
		const sint64 offset = static_cast<sint16>(instr.Imm());
		const auto resolvedLabel = x86Asm.newLabel();

		if (rs == 0)
			x86Asm.xor_(x86::rax, x86::rax);
		else
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
		x86Asm.mov(x86::rax, reinterpret_cast<uint64>(&handleResolvingError<access, 0>));
		x86Asm.call(x86::rax); // handling
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel); // goto @end
		x86Asm.bind(resolvedLabel); // @resolved
		// now, paddr is stored in rax
	}

	template <Opcode op, OpRegimm sub, typename Instr> [[nodiscard]]
	static DecodedToken branchOffsetInternal(const AssembleContext& ctx, Instr instr)
	{
		constexpr BranchType branchType =
			op == Opcode::BEQ ||
			op == Opcode::BNE ||
			sub == OpRegimm::BLTZ ||
			sub == OpRegimm::BLTZAL ||
			op == Opcode::BLEZ ||
			op == Opcode::BGTZ ||
			sub == OpRegimm::BGEZ ||
			sub == OpRegimm::BGEZAL
			? BranchType::Normal
			: BranchType::Likely;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		auto&& pc = ctx.cpu->GetPc().Raw();
		const sint64 offset = static_cast<sint64>(static_cast<sint16>(instr.Imm())) * 4;

		if (instr.Rs() != 0)
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64_t>(&gpr.Raw()[instr.Rs()])));
		else
			x86Asm.xor_(x86::rax, x86::rax);

		if constexpr (
			op == Opcode::BEQ || op == Opcode::BEQL ||
			op == Opcode::BNE || op == Opcode::BNEL)
		{
			x86Asm.mov(x86::rcx, x86::rax);
			if (instr.Rt() != 0)
				x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64_t>(&gpr.Raw()[instr.Rt()])));
			else
				x86Asm.xor_(x86::rax, x86::rax);
			x86Asm.cmp(x86::rax, x86::rcx);
			if constexpr (op == Opcode::BEQ || op == Opcode::BEQL) x86Asm.sete(x86::r8);
			else x86Asm.setne(x86::r8);
		}
		else if constexpr (sub == OpRegimm::BLTZ || sub == OpRegimm::BLTZL || sub == OpRegimm::BLTZAL)
		{
			x86Asm.shr(x86::rax, 63);
			x86Asm.mov(x86::r8, x86::rax);
		}
		else if constexpr (op == Opcode::BLEZ || op == Opcode::BLEZL)
		{
			x86Asm.test(x86::rax, x86::rax);
			x86Asm.setle(x86::r8);
		}
		else if constexpr (op == Opcode::BGTZ || op == Opcode::BGTZL)
		{
			x86Asm.test(x86::rax, x86::rax);
			x86Asm.setg(x86::r8);
		}
		else if constexpr (
			sub == OpRegimm::BGEZ || sub == OpRegimm::BGEZL ||
			sub == OpRegimm::BGEZAL || sub == OpRegimm::BGEZALL)
		{
			x86Asm.shr(x86::rax, 63);
			x86Asm.xor_(x86::rax, 1);
			x86Asm.mov(x86::r8, x86::rax);
		}
		else static_assert(AlwaysFalseValue<OpRegimm, sub>);
		// now, r8 <- condition
		x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&pc.curr)));
		x86Asm.add(x86::rax, offset);
		x86Asm.mov(x86::rdx, x86::rax); // rdx <- vaddr
		x86Asm.mov(x86::rcx, (uint64)ctx.cpu); // rcx <- *cpu
		x86Asm.mov(x86::rax, &Process::BranchVAddr64<branchType>);
		x86Asm.call(x86::rax);
		if constexpr (sub == OpRegimm::BLTZAL || sub == OpRegimm::BGEZAL || sub == OpRegimm::BGEZALL)
		{
			// link register
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.add(x86::rax, 4);
			x86Asm.mov(x86::qword_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetGpr().Raw()[GprRA_31])), x86::rax);
		}
		return branchType == BranchType::Normal ? DecodedToken::Branch : DecodedToken::BranchLikely;
	}

	N64_ABI static void helperDMULT(sint64 rs, sint64 rt, uint64* lo, uint64* hi)
	{
		const int128 result = (int128)rs * (int128)rt;
		*lo = static_cast<uint64>(result & 0xFFFFFFFFFFFFFFFF);
		*hi = static_cast<uint64>(result >> 64);
	}

	N64_ABI static void helperDMULTU(uint64 rs, uint64 rt, uint64* lo, uint64* hi)
	{
		const uint128 result = (uint128)rs * (uint128)rt;
		*lo = static_cast<uint64>(result & 0xFFFFFFFFFFFFFFFF);
		*hi = static_cast<uint64>(result >> 64);;
	}

	N64_ABI static void helperDDIV(sint64 rs, sint64 rt, uint64* lo, uint64* hi)
	{
		const sint64 dividend = rs;
		const sint64 divisor = rt;
		if (dividend == 0x8000000000000000 && divisor == 0xFFFFFFFFFFFFFFFF)
		{
			*lo = dividend;
			*hi = 0;
		}
		else if (divisor == 0)
		{
			*hi = dividend;
			*lo = dividend >= 0 ? -1 : 1;
		}
		else
		{
			const sint64 quotient = dividend / divisor;
			const sint64 remainder = dividend % divisor;
			*lo = quotient;
			*hi = remainder;
		}
	}

	N64_ABI static void helperDDIVU(uint64 rs, uint64 rt, uint64* lo, uint64* hi)
	{
		const uint64 dividend = rs;
		const uint64 divisor = rt;
		if (divisor == 0)
		{
			*lo = -1;
			*hi = dividend;
		}
		else
		{
			const uint64 quotient = dividend / divisor;
			const uint64 remainder = dividend % divisor;
			*lo = quotient;
			*hi = remainder;
		}
	}

	template <Opcode op, BusAccess access> [[nodiscard]]
	static DecodedToken memoryAccessShifted(const AssembleContext& ctx, const AssembleState& state, InstructionFi instr)
	{
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.cpu->GetGpr();
		const sint64 offset = static_cast<sint16>(instr.Offset());
		const auto resolvedLabel = x86Asm.newLabel();

		x86Asm.mov(x86::rcx, (uint64)ctx.n64); // rcx <- *n64
		x86Asm.mov(x86::rdx, (uint64)ctx.cpu); // rdx <- *cpu
		x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[instr.Base()])));
		x86Asm.add(x86::rax, offset);
		x86Asm.mov(x86::r8, x86::rax); // r8 <- vaddr
		if constexpr (access == BusAccess::Load)
		{
			x86Asm.mov(x86::r9, (uint64)&gpr.Raw()[instr.Rt()]); // r9 <- *rt
			x86Asm.call(reinterpret_cast<uint64>(&helperL_loadShifted<op>));
		}
		else if constexpr (access == BusAccess::Store)
		{
			x86Asm.mov(x86::rax, x86::qword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[instr.Rt()])));
			x86Asm.mov(x86::r9, x86::rax); // r9 <- rt
			x86Asm.call(reinterpret_cast<uint64>(&helperS_storeShifted<op>));
		}
		else static_assert(AlwaysFalseValue<BusAccess, access>);

		x86Asm.cmp(x86::al, 0);
		x86Asm.jne(resolvedLabel);
		// now, error occured
		x86Asm.mov(x86::rax, state.recompiledLength);
		x86Asm.jmp(ctx.endLabel);
		x86Asm.bind(resolvedLabel); // @resolved
		return DecodedToken::Continue;
	}

	template <Opcode op>
	N64_ABI static bool helperL_loadShifted(N64System& n64, Cpu& cpu, uint64 vaddr, uint64* rt)
	{
		const auto paddr = Mmu::ResolveVAddr(cpu, vaddr);
		if (paddr.has_value() == false)
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			Process::ThrowException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Load>(), 0);
			return false;
		}

		if constexpr (op == Opcode::LDL)
		{
			const sint32 shift = 8 * ((vaddr ^ 0) & 7);
			const uint64 mask = (uint64)0xFFFFFFFFFFFFFFFF << shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			const uint64 oldRt = *rt;
			if (rt != &cpu.GetGpr().Raw()[0])
				*rt = (oldRt & ~mask) | (data << shift);
		}
		else if constexpr (op == Opcode::LDR)
		{
			const sint32 shift = 8 * ((vaddr ^ 7) & 7);
			const uint64 mask = (uint64)0xFFFFFFFFFFFFFFFF >> shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			const uint64 oldRt = *rt;
			if (rt != &cpu.GetGpr().Raw()[0])
				*rt = (oldRt & ~mask) | (data >> shift);
		}
		else if constexpr (op == Opcode::LWL)
		{
			const uint32 shift = 8 * ((vaddr ^ 0) & 3);
			const uint32 mask = 0xFFFFFFFF << shift;
			const uint32 data = PAddr32(paddr.value() & ~3);
			const sint32 result = (*rt & ~mask) | data << shift;
			if (rt != &cpu.GetGpr().Raw()[0])
				*rt = static_cast<sint64>(result);
		}
		else if constexpr (op == Opcode::LWR)
		{
			const uint32 shift = 8 * ((vaddr ^ 3) & 3);
			const uint32 mask = 0xFFFFFFFF >> shift;
			const uint32 data = PAddr32(paddr.value() & ~3);
			const sint32 result = (*rt & ~mask) | data >> shift;
			if (rt != &cpu.GetGpr().Raw()[0])
				*rt = static_cast<sint64>(result);
		}
		else static_assert(AlwaysFalseValue<Opcode, op>);
		return true;
	}

	template <Opcode op>
	N64_ABI static bool helperS_storeShifted(N64System& n64, Cpu& cpu, uint64 vaddr, uint64 rt)
	{
		const auto paddr = Mmu::ResolveVAddr(cpu, vaddr);
		if (paddr.has_value() == false)
		{
			cpu.GetCop0().HandleTlbException(vaddr);
			Process::ThrowException(cpu, cpu.GetCop0().GetTlbExceptionCode<BusAccess::Store>(), 0);
			return false;
		}

		if constexpr (op == Opcode::SDL)
		{
			const sint32 shift = 8 * ((vaddr ^ 0) & 7);
			const uint64 mask = 0xFFFFFFFFFFFFFFFF >> shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			Mmu::WritePaddr64(n64, PAddr32(paddr.value() & ~7), (data & ~mask) | ((rt) >> shift));
		}
		else if constexpr (op == Opcode::SDR)
		{
			const sint32 shift = 8 * ((vaddr ^ 7) & 7);
			const uint64 mask = (uint64)0xFFFFFFFFFFFFFFFF << shift;
			const uint64 data = Mmu::ReadPaddr64(n64, PAddr32(paddr.value() & ~7));
			Mmu::WritePaddr64(n64, PAddr32(paddr.value() & ~7), (data & ~mask) | (rt << shift));
		}
		else if constexpr (op == Opcode::SWL)
		{
			const uint32 shift = 8 * ((vaddr ^ 0) & 3);
			const uint32 mask = 0xFFFFFFFF >> shift;
			const uint32 data = Mmu::ReadPaddr32(n64, PAddr32(paddr.value() & ~3));
			Mmu::WritePaddr32(n64, PAddr32(paddr.value() & ~3), (data & ~mask) | (rt >> shift));
		}
		else if constexpr (op == Opcode::SWR)
		{
			const uint32 shift = 8 * ((vaddr ^ 3) & 3);
			const uint32 mask = 0xFFFFFFFF << shift;
			const uint32 data = Mmu::ReadPaddr32(n64, PAddr32(paddr.value() & ~3));
			Mmu::WritePaddr32(n64, PAddr32(paddr.value() & ~3), (data & ~mask) | rt << shift);
		}
		else static_assert(AlwaysFalseValue<Opcode, op>);
		return true;
	}
};
