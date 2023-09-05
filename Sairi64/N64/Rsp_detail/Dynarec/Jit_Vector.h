#pragma once

#include "Jit.h"

// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core

namespace N64::Rsp_detail::Dynarec
{
}

class N64::Rsp_detail::Dynarec::Jit::Vector
{
public:
	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L603
	[[nodiscard]]
	static DecodedToken CTC2(const AssembleContext& ctx, InstructionCop2VecSub instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		const uint8 vt = instr.Vt();
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[vt])));
		x86Asm.mov(x86::dx, x86::ax);
		x86Asm.mov(x86::rcx, (uint64)&Process::AccessVU(rsp));
		const uint8 control = instr.Vs() & 3;
		auto helper =
			control == 0 ? helperCTC2<VuControl::VcO> :
			control == 1 ? helperCTC2<VuControl::VcC> :
			helperCTC2<VuControl::VcE>;
		x86Asm.mov(x86::rax, (uint64)helper);
		x86Asm.call(x86::rax);
		return DecodedToken::Continue;
	}

	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L1668
	[[nodiscard]]
	static DecodedToken VXOR(const AssembleContext& ctx, InstructionCop2VecFunct instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		auto&& vu = Process::AccessVU(rsp);

		x86Asm.mov(x86::rcx, (uint64)&vu);
		x86Asm.mov(x86::rdx, (uint64)&vu.regs[instr.Vd()]);
		x86Asm.mov(x86::r8, (uint64)&vu.regs[instr.Vs()]);
		x86Asm.mov(x86::r9, (uint64)&vu.regs[instr.Vt()]);
		x86Asm.mov(x86::al, instr.Element());
		x86Asm.mov(x86::byte_ptr(x86::rsp, 32), x86::al);
		x86Asm.mov(x86::rax, (uint64)&helperVXOR);
		x86Asm.call(x86::rax);
		return DecodedToken::Continue;
	}

	template <OpLwc2Funct funct> [[nodiscard]]
	static DecodedToken LWC2_funct(const AssembleContext& ctx, InstructionLv instr)
	{
		JIT_SP;
		return wordCop2Funct<funct, OpSwc2Funct::Invalid_0xFF>(ctx, instr);
	}

	template <OpSwc2Funct funct> [[nodiscard]]
	static DecodedToken SWC2_funct(const AssembleContext& ctx, InstructionSv instr)
	{
		JIT_SP;
		return wordCop2Funct<OpLwc2Funct::Invalid_0xFF, funct>(ctx, instr);
	}

private:
	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L141
	static sint32 signExtend7bitOffset(uint8 offset, uint8 shiftAmount)
	{
		const sint8 signedOffset = ((offset << 1) & 0x80) | offset;
		const sint32 offset1 = (sint32)signedOffset;
		const uint32 offset2 = offset1;
		return static_cast<sint32>(offset2 << shiftAmount);
	}

	template <VuControl control>
	N64_ABI static void helperCTC2(VU& vu, uint16 value)
	{
		if constexpr (control == VuControl::VcO)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcO.h.elements[VuElementIndex(i)] = VuFlag16(((value >> (i + 8)) & 1) == 1);
				vu.vcO.l.elements[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
		else if constexpr (control == VuControl::VcC)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcC.h.elements[VuElementIndex(i)] = VuFlag16(((value >> (i + 8)) & 1) == 1);
				vu.vcC.l.elements[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
		else if constexpr (control == VuControl::VcE)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcE.elements[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
	}

	template <OpLwc2Funct lwc2, OpSwc2Funct swc2, typename Instr> [[nodiscard]]
	static DecodedToken wordCop2Funct(const AssembleContext& ctx, Instr instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		auto&& vu = Process::AccessVU(rsp);

		const uint8 e = instr.Element();
		constexpr uint8 shiftAmount = lwc2 != OpLwc2Funct::Invalid_0xFF
		                              ? Lwc2FunctShiftAmount<lwc2>()
		                              : Swc2FunctShiftAmount<swc2>();
		const sint32 offset = signExtend7bitOffset(instr.Offset(), shiftAmount);

		x86Asm.mov(x86::rcx, (uint64)&rsp.Dmem()); // rcx <- *dmem
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[instr.Base()])));
		x86Asm.add(x86::eax, offset);
		x86Asm.mov(x86::edx, x86::eax); // edx <- startAddr
		x86Asm.mov(x86::r8, reinterpret_cast<uint64>(&vu.regs[instr.Vt()])); // r8 <- *vt
		x86Asm.mov(x86::r9b, e); // r9b <- e
		if constexpr (lwc2 == OpLwc2Funct::LQV) x86Asm.call((uint64)&helperLQV);
		else if constexpr (swc2 == OpSwc2Funct::SQV) x86Asm.call((uint64)&helperSQV);
		else static_assert(AlwaysFalseValue<OpLwc2Funct, swc2>);

		return DecodedToken::Continue;
	}

	N64_ABI static void helperVXOR(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 e)
	{
		const Vpr_t vte = GetVtE(vt, e);
		for (int i = 0; i < 8; ++i)
		{
			vu.accum.l.elements[i] = vte.elements[i] ^ vs.elements[i];
			vd.elements[i] = vu.accum.l.elements[i];
		}
	}

	N64_ABI static void helperLQV(const SpDmem& dmem, uint32 startAddr, Vpr_t& vt, uint8 e)
	{
		const uint32 endAddr = ((startAddr & ~15) + 15);
		for (int i = 0; startAddr + i <= endAddr && i + e < 16; ++i)
		{
			vt.bytes[VuByteIndex(i + e)] = dmem.ReadSpByte(startAddr + i);
		}
	}

	N64_ABI static void helperSQV(SpDmem& dmem, uint32 startAddr, const Vpr_t& vt, uint8 e)
	{
		const uint32 endAddr = ((startAddr & ~15) + 15);
		for (int i = 0; startAddr + i <= endAddr; ++i)
		{
			dmem.WriteSpByte(startAddr + i, vt.bytes[VuByteIndex((i + e) & 15)]);
		}
	}
};
