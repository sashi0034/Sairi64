#pragma once

#include "Jit.h"

// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core

namespace N64::Rsp_detail::Dynarec
{
}

class N64::Rsp_detail::Dynarec::Jit::Vector
{
public:
	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L581
	[[nodiscard]]
	static DecodedToken CFC2(const AssembleContext& ctx, InstructionCop2VecSub instr)
	{
		JIT_SP;
		const uint8 vt = instr.Vt();
		if (vt == 0) return DecodedToken::Continue;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		x86Asm.mov(x86::rcx, (uint64)&Process::AccessVU(rsp));
		const uint8 control = instr.Vs() & 3;
		auto helper =
			control == 0 ? helperCFC2<VuControl::VcO> :
			control == 1 ? helperCFC2<VuControl::VcC> :
			helperCFC2<VuControl::VcE>;
		x86Asm.call((uint64)helper);
		x86Asm.mov(x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[vt])), x86::eax);
		return DecodedToken::Continue;
	}

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
		x86Asm.call((uint64)helper);
		return DecodedToken::Continue;
	}

	[[nodiscard]]
	static DecodedToken VSAR(const AssembleContext& ctx, InstructionCop2VecFunct instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		const uint8 e = instr.Element();
		auto helper =
			e == 0x8 ? helperVSAR<0x8> :
			e == 0x9 ? helperVSAR<0x9> :
			e == 0xA ? helperVSAR<0xA> :
			helperVSAR<0>;
		if (helper != helperVSAR<0>) x86Asm.mov(x86::rcx, (uint64)&Process::AccessVU(rsp));
		x86Asm.mov(x86::rdx, (uint64)&Process::AccessVU(rsp).regs[instr.Vd()]);
		x86Asm.call((uint64)helper);
		return DecodedToken::Continue;
	}

	// https://github.com/Dillonb/n64/blob/cccc33fd1b7cbc08588206dccbe077e17b642f88/src/cpu/rsp_vector_instructions.c#L1668
	template <OpCop2VecFunct funct> [[nodiscard]]
	static DecodedToken CP2_arithmetic(const AssembleContext& ctx, InstructionCop2VecFunct instr)
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
		x86Asm.call(reinterpret_cast<uint64>(&helperCP2_arithmetic<funct>));
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

	// https://github.com/SimoneN64/Kaizen/blob/56ab73865271635d887eab96a0e51873347abe77/src/backend/core/rsp/instructions.cpp#L705
	static sint16 signedClamp(sint64 value)
	{
		// if ((value & 0xFFFF) == 0) return value; // TODO: あってるか検証
		// if (value < 0) return -32768;
		// return 32767;
		if (value < -32768) return -32768;
		if (value > 32767) return 32767;
		return static_cast<int16_t>(value);
	}

	static uint16 unsignedClamp(sint64 value)
	{
		if (value < 0) return 0;
		if (value > 32767) return 65535;
		return value;
	}

	template <VuControl control>
	N64_ABI static sint32 helperCFC2(VU& vu)
	{
		uint16 value{};
		if constexpr (control == VuControl::VcO)
		{
			for (int i = 0; i < 8; i++)
			{
				const bool h = vu.vcO.h.uE[VuElementIndex(i)] != 0;
				const bool l = vu.vcO.l.uE[VuElementIndex(i)] != 0;
				const uint32 mask = (l << i) | (h << (i + 8));
				value |= mask;
			}
			return (sint32)static_cast<sint16>(value);
		}
		else if constexpr (control == VuControl::VcC)
		{
			for (int i = 0; i < 8; i++)
			{
				const bool h = vu.vcC.h.uE[VuElementIndex(i)] != 0;
				const bool l = vu.vcC.l.uE[VuElementIndex(i)] != 0;
				const uint32 mask = (l << i) | (h << (i + 8));
				value |= mask;
			}
			return (sint32)static_cast<sint16>(value);
		}
		else if constexpr (control == VuControl::VcE)
		{
			for (int i = 0; i < 8; i++)
			{
				const bool l = vu.vcE.uE[VuElementIndex(i)] != 0;
				value |= (l << i);
			}
			return (sint32)static_cast<sint16>(value);
		}
		else
		{
			static_assert(AlwaysFalseValue<VuControl, control>);
			return {};
		}
	}

	template <VuControl control>
	N64_ABI static void helperCTC2(VU& vu, uint16 value)
	{
		if constexpr (control == VuControl::VcO)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcO.h.uE[VuElementIndex(i)] = VuFlag16(((value >> (i + 8)) & 1) == 1);
				vu.vcO.l.uE[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
		else if constexpr (control == VuControl::VcC)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcC.h.uE[VuElementIndex(i)] = VuFlag16(((value >> (i + 8)) & 1) == 1);
				vu.vcC.l.uE[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
		else if constexpr (control == VuControl::VcE)
		{
			for (int i = 0; i < 8; ++i)
			{
				vu.vcE.uE[VuElementIndex(i)] = VuFlag16(((value >> i) & 1) == 1);
			}
		}
	}

	template <OpLwc2Funct lwc2, OpSwc2Funct swc2, typename Instr> [[nodiscard]]
	static DecodedToken wordCop2Funct(const AssembleContext& ctx, Instr instr)
	{
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

	template <OpCop2VecFunct funct>
	N64_ABI static void helperCP2_arithmetic(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 e)
	{
		const Vpr_t vte = GetVtE(vt, e);
		for (int i = 0; i < 8; ++i)
		{
			if constexpr (funct == OpCop2VecFunct::VADD)
			{
				const sint32 result = vs.sE[i] + vte.sE[i] + (vu.vcO.l.sE[i] != 0);
				vu.accum.l.uE[i] = result;
				vd.uE[i] = static_cast<uint16>(signedClamp(result));
				vu.vcO.l.uE[i] = 0;
				vu.vcO.h.uE[i] = 0;
			}
			else if constexpr (funct == OpCop2VecFunct::VXOR)
			{
				vu.accum.l.uE[i] = vte.uE[i] ^ vs.uE[i];
				vd.uE[i] = vu.accum.l.uE[i];
			}
			else static_assert(AlwaysFalseValue<OpCop2VecFunct, funct>);
		}
	}

	template <uint8 e>
	N64_ABI static void helperVSAR(VU& vu, Vpr_t& vd)
	{
		if constexpr (e == 0x8)
		{
			vd.single = vu.accum.h.single;
		}
		else if constexpr (e == 0x9)
		{
			vd.single = vu.accum.m.single;
		}
		else if constexpr (e == 0xA)
		{
			vd.single = vu.accum.l.single;
		}
		else
		{
			vd.single = 0;
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
