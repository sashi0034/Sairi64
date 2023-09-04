﻿#pragma once

#include "JitForward.h"
#include "N64/Instruction.h"
#ifndef RSP_PROCESS_INTERNAL
#error "This file is an internal file used by SpRecompiler"
#endif

#define JIT_SP N64_TRACE(U"JIT-SP => " + instr.Stringify())

// https://hack64.net/docs/VR43XX.pdf

namespace N64::Rsp_detail::Dynarec
{
	class Jit;

	constexpr uint32 StackSizeRequirement = 56;
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

	template <Opcode op> [[nodiscard]]
	static DecodedToken I_immediateArithmetic(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.rsp->GetGpr();
		const uint8 rs = instr.Rs();
		const uint16 imm = instr.Imm();

		if (rs == 0)
			x86Asm.xor_(x86::eax, x86::eax); // eax <- 0
		else
			x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs]))); // eax <- rs

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

		x86Asm.mov(x86::rcx, (uint64)&gpr.Raw()[rt]);
		x86Asm.mov(x86::dword_ptr(x86::rcx), x86::eax);

		return DecodedToken::Continue;
	}

	template <Opcode op> [[nodiscard]]
	static DecodedToken L_load(const AssembleContext& ctx, InstructionI instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if (rt == 0) return DecodedToken::Continue;

		auto&& x86Asm = *ctx.x86Asm;
		auto&& gpr = ctx.rsp->GetGpr();
		const uint8 rs = instr.Rs();
		const sint32 offset = (sint32)static_cast<sint16>(instr.Imm());

		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rs])));
		x86Asm.mov(x86::edx, x86::eax);
		x86Asm.add(x86::edx, offset); // edx <- address
		x86Asm.mov(x86::rcx, (uint64)&ctx.rsp->Dmem()); // rcx <- *dmem

		if constexpr (op == Opcode::LW || op == Opcode::LWU)
		{
			x86Asm.mov(x86::rax, &readDmem<uint32>);
			x86Asm.call(x86::rax);
		}
		else static_assert(AlwaysFalseValue<Opcode, op>);

		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&gpr.Raw()[rt])), x86::eax);
		return DecodedToken::Continue;
	}

	class Vector;

private:
	template <typename Wire>
	N64_ABI static Wire readDmem(SpDmem& dmem, uint32 addr)
	{
		return dmem.ReadSpData<Wire>(addr & 0xFFF);
	}

	template <typename Wire>
	N64_ABI static void writeDmem(SpDmem& dmem, uint32 addr, Wire value)
	{
		dmem.WriteSpData<Wire>(addr & 0xFFF, value);
	}
};
