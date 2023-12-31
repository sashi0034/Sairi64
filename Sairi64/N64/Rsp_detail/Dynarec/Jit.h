﻿#pragma once

#include "JitForward.h"
#include "N64/Instruction.h"
#include "N64/Rsp_detail/Rsp_Interface.h"

#ifndef RSP_PROCESS_INTERNAL
#error "This file is an internal file used by SpRecompiler"
#endif

#define JIT_SP N64_TRACE(Rsp, U"JIT-SP => " + instr.Stringify())

// https://hack64.net/docs/VR43XX.pdf

namespace N64::Rsp_detail::Dynarec
{
	class Jit;

	constexpr uint32 StackSizeRequirement = 56; // 関数の引数は6個まで

	constexpr uint8 GprLR_31 = 31;
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
		const uint32 imm = (uint16)instr.Imm() << 16;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(*ctx.rsp)[rt])), imm);
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SLT_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const uint8 rt = instr.Rt();

		x86Asm.mov(x86::rax, (uint64)&gpr[0]);
		loadGpr32(x86Asm, x86::ecx, x86::rax, rs); // rcx <- rs
		loadGpr32(x86Asm, x86::edx, x86::rax, rt); // rdx <- rt

		x86Asm.xor_(x86::r8, x86::r8);
		x86Asm.cmp(x86::ecx, x86::edx);
		if constexpr (funct == OpSpecialFunct::SLT)
			x86Asm.setl(x86::r8b);
		else if constexpr (funct == OpSpecialFunct::SLTU)
			x86Asm.setb(x86::r8b);
		else
			static_assert(AlwaysFalseValue<Opcode, funct>);
		x86Asm.mov(x86::dword_ptr(x86::rax, rd * 4), x86::r8); // gpr[rd] <- rcx
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken SLTI_template(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const uint16 imm = instr.Imm();

		if (rs == 0)
			x86Asm.xor_(x86::eax, x86::eax); // rax <- 0
		else
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rs]))); // rax <- rs
		x86Asm.xor_(x86::edx, x86::edx);
		x86Asm.cmp(x86::eax, (sint32)static_cast<sint16>(imm));
		if constexpr (op == Opcode::SLTI)
			x86Asm.setl(x86::dl);
		else if constexpr (op == Opcode::SLTIU)
			x86Asm.setb(x86::dl);
		else
			static_assert(AlwaysFalseValue<Opcode, op>);
		x86Asm.mov(x86::rcx, (uint64)&gpr[rt]); // rcx <- *rt
		x86Asm.mov(x86::dword_ptr(x86::rcx), x86::edx); // rcx <- rax
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken I_immediateArithmetic(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const uint16 imm = instr.Imm();

		if (rs == 0)
			x86Asm.xor_(x86::eax, x86::eax); // eax <- 0
		else
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rs]))); // eax <- rs

		if constexpr (op == Opcode::ADDI || op == Opcode::ADDIU)
		{
			x86Asm.add(x86::eax, (sint32)static_cast<sint16>(imm)); // eax <- eax + immediate
		}
		else if constexpr (op == Opcode::ANDI)
		{
			x86Asm.and_(x86::eax, (uint32)imm);
		}
		else if constexpr (op == Opcode::ORI)
		{
			x86Asm.or_(x86::eax, (uint32)imm);
		}
		else if constexpr (op == Opcode::XORI)
		{
			x86Asm.xor_(x86::eax, (uint32)imm);
		}
		else
		{
			static_assert(AlwaysFalseValue<Opcode, op>);
		}
		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rt])), x86::eax);

		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SPECIAL_arithmetic(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const uint8 rt = instr.Rt();

		x86Asm.mov(x86::rax, (uint64)&gpr[0]);
		loadGpr32(x86Asm, x86::ecx, x86::rax, rs); // rcx <- rs
		loadGpr32(x86Asm, x86::edx, x86::rax, rt); // rdx <- rt

		if constexpr (funct == OpSpecialFunct::ADD || funct == OpSpecialFunct::ADDU)
		{
			x86Asm.add(x86::ecx, x86::edx);
		}
		else if constexpr (funct == OpSpecialFunct::SUB || funct == OpSpecialFunct::SUBU)
		{
			x86Asm.sub(x86::ecx, x86::edx);
		}
		else if constexpr (funct == OpSpecialFunct::AND)
		{
			x86Asm.and_(x86::ecx, x86::edx);
		}
		else if constexpr (funct == OpSpecialFunct::OR)
		{
			x86Asm.or_(x86::ecx, x86::edx);
		}
		else if constexpr (funct == OpSpecialFunct::XOR)
		{
			x86Asm.xor_(x86::ecx, x86::edx);
		}
		else if constexpr (funct == OpSpecialFunct::NOR)
		{
			x86Asm.or_(x86::ecx, x86::edx);
			x86Asm.not_(x86::ecx);
		}
		else static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);

		x86Asm.mov(x86::dword_ptr(x86::rax, rd * 4), x86::ecx); // gpr[rd] <- ecx
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SPECIAL_shift(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rt = instr.Rt();
		const uint8 sa = instr.Sa();

		if (rt != 0)
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rt]))); // eax <- rt
		else
			x86Asm.xor_(x86::eax, x86::eax);

		if constexpr (funct == OpSpecialFunct::SLL)
		{
			x86Asm.shl(x86::eax, sa);
		}
		else if constexpr (funct == OpSpecialFunct::SRL)
		{
			x86Asm.shr(x86::eax, sa);
		}
		else if constexpr (funct == OpSpecialFunct::SRA)
		{
			x86Asm.sar(x86::eax, sa);
		}
		else static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);

		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rd])), x86::eax); // gpr[rd] <- eax
		return DecodedToken::Continue;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken SPECIAL_shiftVariable(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		const uint8 rd = instr.Rd();
		if (rd == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rt = instr.Rt();
		const uint8 rs = instr.Rs();

		if (rt != 0)
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rt]))); // eax <- rt
		else
			x86Asm.xor_(x86::eax, x86::eax);
		if (rs != 0)
		{
			x86Asm.mov(x86::ecx, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rs]))); // ecx <- rs
			x86Asm.and_(x86::ecx, 0b11111);
		}
		else
			x86Asm.xor_(x86::ecx, x86::ecx);

		if constexpr (funct == OpSpecialFunct::SLLV)
		{
			x86Asm.shl(x86::eax, x86::cl);
		}
		else if constexpr (funct == OpSpecialFunct::SRLV)
		{
			x86Asm.shr(x86::eax, x86::cl);
		}
		else if constexpr (funct == OpSpecialFunct::SRAV)
		{
			x86Asm.sar(x86::eax, x86::cl);
		}
		else static_assert(Utils::AlwaysFalseValue<OpSpecialFunct, funct>);

		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rd])), x86::eax); // gpr[rd] <- eax
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken L_load(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const sint32 offset = (sint32)static_cast<sint16>(instr.Imm());

		if (rs != 0)
		{
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rs])));
			x86Asm.mov(x86::edx, x86::eax);
		}
		else
		{
			x86Asm.xor_(x86::edx, x86::edx);
		}
		x86Asm.add(x86::edx, offset); // edx <- address
		x86Asm.mov(x86::rcx, (uint64)&ctx.rsp->Dmem()); // rcx <- *dmem

		if constexpr (op == Opcode::LW || op == Opcode::LWU)
		{
			x86Asm.mov(x86::rax, &readDmem<uint32>);
			x86Asm.call(x86::rax);
		}
		else if constexpr (op == Opcode::LH || op == Opcode::LHU)
		{
			x86Asm.mov(x86::rax, &readDmem<uint16>);
			x86Asm.call(x86::rax);
			if constexpr (op == Opcode::LH) x86Asm.movsx(x86::eax, x86::ax);
			else x86Asm.movzx(x86::eax, x86::ax);
		}
		else if constexpr (op == Opcode::LB || op == Opcode::LBU)
		{
			x86Asm.mov(x86::rax, &readDmem<uint8>);
			x86Asm.call(x86::rax);
			if constexpr (op == Opcode::LB) x86Asm.movsx(x86::eax, x86::al);
			else x86Asm.movzx(x86::eax, x86::al);
		}
		else static_assert(AlwaysFalseValue<Opcode, op>);

		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rt])), x86::eax);
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken S_store(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		const uint8 rs = instr.Rs();
		const uint8 rt = instr.Rt();
		const sint32 offset = (sint32)static_cast<sint16>(instr.Imm());

		if (rt != 0)
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rt])));
		else
			x86Asm.xor_(x86::eax, x86::eax);
		x86Asm.mov(x86::r8d, x86::eax); // r8d <- value
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr[rs])));
		x86Asm.mov(x86::edx, x86::eax);
		x86Asm.add(x86::edx, offset); // edx <- address
		x86Asm.mov(x86::rcx, (uint64)&ctx.rsp->Dmem()); // rcx <- *dmem

		if constexpr (op == Opcode::SW)
		{
			x86Asm.call(&writeDmem<uint32>);
		}
		else if constexpr (op == Opcode::SH)
		{
			x86Asm.call(&writeDmem<uint16>);
		}
		else if constexpr (op == Opcode::SB)
		{
			x86Asm.call(&writeDmem<uint8>);
		}
		else static_assert(AlwaysFalseValue<Opcode, op>);
		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken J_template(const AssembleContext& ctx, InstructionJ instr)
	{
		JIT_SP;
		static_assert(op == Opcode::J || op == Opcode::JAL);
		auto&& x86Asm = *ctx.x86Asm;
		auto&& pc = Process::AccessPc(*ctx.rsp);
		if constexpr (op == Opcode::JAL)
		{
			// link register
			x86Asm.mov(x86::ax, x86::word_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.add(x86::ax, 4);
			x86Asm.movzx(x86::eax, x86::ax);;
			x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(*ctx.rsp)[GprLR_31])), x86::eax);
		}
		const uint16 target = instr.Target() << 2;
		x86Asm.mov(x86::word_ptr(reinterpret_cast<uint64>(&pc.next)), target & SpPcMask_0xFFC);
		return DecodedToken::Branch;
	}

	template <OpSpecialFunct funct> [[nodiscard]]
	static DecodedToken JR_template(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		static_assert(funct == OpSpecialFunct::JR || funct == OpSpecialFunct::JALR);
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		const uint8 rs = instr.Rs();
		if (rs != 0)
		{
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[rs])));
			x86Asm.and_(x86::ax, SpPcMask_0xFFC);
		}
		else
		{
			x86Asm.xor_(x86::eax, x86::eax);
		}
		x86Asm.mov(x86::word_ptr(reinterpret_cast<uint64>(&Process::AccessPc(rsp).next)), x86::ax);
		if constexpr (funct == OpSpecialFunct::JALR)
		{
			// link register
			const uint8 rd = instr.Rd();
			if (rd == 0) return DecodedToken::Branch;
			x86Asm.mov(x86::ax, x86::word_ptr(reinterpret_cast<uint64>(&Process::AccessPc(rsp).curr)));
			x86Asm.movzx(x86::eax, x86::ax);
			x86Asm.add(x86::eax, 4);
			x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[rd])), x86::eax);
		}
		return DecodedToken::Branch;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken B_branchOffset(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		return branchOffsetInternal<op, OpRegimm::Invalid_0xFF>(ctx, instr);
	}

	template <OpRegimm sub> [[nodiscard]]
	static DecodedToken B_branchOffset(const AssembleContext& ctx, InstructionRegimm instr)
	{
		JIT_SP;
		return branchOffsetInternal<Opcode::Invalid_0xFF, sub>(ctx, instr);
	}

	[[nodiscard]]
	static DecodedToken MTC0(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)ctx.n64);
		x86Asm.mov(x86::rdx, (uint64)ctx.rsp);
		x86Asm.mov(x86::r8b, instr.Rd());
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(*ctx.rsp)[instr.Rt()])));
		x86Asm.mov(x86::r9d, x86::eax);
		x86Asm.call((uint64)&Rsp::Interface::WriteSpCop0);
		return DecodedToken::Continue;
	}

	[[nodiscard]]
	static DecodedToken MFC0(const AssembleContext& ctx, InstructionCopSub instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		const uint8 rd = instr.Rd();
		if (rt == 0 && rd != SpCop0::Semaphore_7) return DecodedToken::Continue;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, (uint64)ctx.n64);
		x86Asm.mov(x86::rdx, (uint64)ctx.rsp);
		x86Asm.mov(x86::r8b, rd);
		x86Asm.call((uint64)&Rsp::Interface::ReadSpCop0);
		if (rt == 0) return DecodedToken::Continue;
		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(*ctx.rsp)[rt])), x86::eax);
		return DecodedToken::Continue;
	}

	[[nodiscard]]
	static DecodedToken BREAK(const AssembleContext& ctx, InstructionR instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, ctx.n64);
		x86Asm.mov(x86::rdx, ctx.rsp);
		x86Asm.call((uint64)&Process::SpBreak);
		return DecodedToken::End;
	}

	class Vector;

private:
	template <typename Wire>
	N64_ABI static Wire readDmem(SpDmem& dmem, uint32 addr)
	{
		return dmem.ReadSpData<Wire>(addr);
	}

	template <typename Wire>
	N64_ABI static void writeDmem(SpDmem& dmem, uint32 addr, Wire value)
	{
		dmem.WriteSpData<Wire>(addr, value);
	}

	static void loadGpr32(
		x86::Assembler& x86Asm,
		const x86::Gpd& dest,
		const x86::Gpq& base,
		const uint8 gprIndex)
	{
		if (gprIndex == 0)
			x86Asm.xor_(dest, dest); // gp <- 0
		else
			x86Asm.mov(dest, x86::dword_ptr(base, gprIndex * 4)); // gp <- gpr[index]
	}

	template <Opcode op, OpRegimm sub, typename Instr> [[nodiscard]]
	static DecodedToken branchOffsetInternal(const AssembleContext& ctx, Instr instr)
	{
		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = Process::AccessGpr(*ctx.rsp);
		auto&& pc = Process::AccessPc(*ctx.rsp);
		const uint16 offset = static_cast<uint16>(instr.Imm() << 2);
		const auto failureLabel = x86Asm.newLabel();

		if (instr.Rs() != 0)
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64_t>(&gpr[instr.Rs()])));
		else
			x86Asm.xor_(x86::eax, x86::eax);

		if constexpr (op == Opcode::BEQ || op == Opcode::BNE)
		{
			x86Asm.mov(x86::ecx, x86::eax);
			if (instr.Rt() != 0)
				x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64_t>(&gpr[instr.Rt()])));
			else
				x86Asm.xor_(x86::eax, x86::eax);
			x86Asm.cmp(x86::eax, x86::ecx);
			if constexpr (op == Opcode::BEQ) x86Asm.sete(x86::r8b);
			else x86Asm.setne(x86::r8b);
		}
		else if constexpr (sub == OpRegimm::BLTZ || sub == OpRegimm::BLTZAL)
		{
			x86Asm.shr(x86::eax, 31);
			x86Asm.mov(x86::r8b, x86::al);
		}
		else if constexpr (op == Opcode::BLEZ)
		{
			x86Asm.test(x86::eax, x86::eax);
			x86Asm.setle(x86::r8b);
		}
		else if constexpr (op == Opcode::BGTZ)
		{
			x86Asm.test(x86::eax, x86::eax);
			x86Asm.setg(x86::r8b);
		}
		else if constexpr (sub == OpRegimm::BGEZ || sub == OpRegimm::BGEZAL)
		{
			x86Asm.shr(x86::eax, 31);
			x86Asm.xor_(x86::al, 1);
			x86Asm.mov(x86::r8b, x86::al);
		}
		else static_assert(AlwaysFalseValue<OpRegimm, sub>);
		// now, r8b <- condition
		x86Asm.test(x86::r8b, x86::r8b); // if condition is false
		x86Asm.je(failureLabel); // then goto @failure
		// now, branch accepted
		x86Asm.mov(x86::ax, x86::word_ptr(reinterpret_cast<uint64>(&pc.curr)));
		x86Asm.add(x86::ax, offset);
		x86Asm.and_(x86::ax, SpPcMask_0xFFC);
		x86Asm.mov(x86::word_ptr(reinterpret_cast<uint64>(&pc.next)), x86::ax);
		// @failure
		x86Asm.bind(failureLabel);
		if constexpr (sub == OpRegimm::BLTZAL || sub == OpRegimm::BGEZAL)
		{
			// link register
			x86Asm.mov(x86::ax, x86::word_ptr(reinterpret_cast<uint64>(&pc.curr)));
			x86Asm.add(x86::ax, 4);
			x86Asm.movzx(x86::eax, x86::ax);;
			x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr[GprLR_31])), x86::eax);
		}
		return DecodedToken::Branch;
	}
};
