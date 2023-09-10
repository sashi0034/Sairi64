#pragma once

#include "Jit.h"
#include "../Vectors.h"

// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core
// https://emudev.org/2020/03/28/RSP.html

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

	template <OpCop2VecFunct funct> [[nodiscard]]
	static DecodedToken VRCP_template(const AssembleContext& ctx, InstructionCop2VecFunct instr)
	{
		JIT_SP;
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		auto&& vu = Process::AccessVU(rsp);
		x86Asm.mov(x86::rcx, (uint64)&rsp);
		x86Asm.mov(x86::rdx, (uint64)&vu.regs[instr.Vt()]);
		x86Asm.mov(x86::r8b, instr.Element());
		x86Asm.mov(x86::r9, (uint64)&vu.regs[instr.Vd()].uE[VuElementIndex(instr.De() & 7)]);
		if constexpr (funct == OpCop2VecFunct::VRCP)
			x86Asm.call((uint64)&helperVRCP);
		else if constexpr (funct == OpCop2VecFunct::VRCPL)
			x86Asm.call((uint64)&helperVRCPL);
		else if constexpr (funct == OpCop2VecFunct::VRCPH)
			x86Asm.call((uint64)&helperVRCPH);
		else static_assert(AlwaysFalseValue<OpCop2VecFunct, funct>);
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
		if constexpr (funct == OpCop2VecFunct::VMUDH)
			x86Asm.call((uint64)&helperVMUDH);
		else if constexpr (funct == OpCop2VecFunct::VMUDL)
			x86Asm.call((uint64)&helperVMUDL);
		else if constexpr (funct == OpCop2VecFunct::VMUDM)
			x86Asm.call((uint64)&helperVMUDM);
		else if constexpr (funct == OpCop2VecFunct::VMUDN)
			x86Asm.call((uint64)&helperVMUDN);
		else if constexpr (funct == OpCop2VecFunct::VMADH)
			x86Asm.call((uint64)&helperVMADH);
		else if constexpr (funct == OpCop2VecFunct::VMADL)
			x86Asm.call((uint64)&helperVMADL);
		else if constexpr (funct == OpCop2VecFunct::VMADM)
			x86Asm.call((uint64)&helperVMADM);
		else if constexpr (funct == OpCop2VecFunct::VMADN)
			x86Asm.call((uint64)&helperVMADN);
		else
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
		const sint32 offset = SignExtend7bitOffset(instr.Offset(), shiftAmount);

		x86Asm.mov(x86::rcx, (uint64)&rsp.Dmem()); // rcx <- *dmem
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[instr.Base()])));
		x86Asm.add(x86::eax, offset);
		x86Asm.mov(x86::edx, x86::eax); // edx <- address
		x86Asm.mov(x86::r8, reinterpret_cast<uint64>(&vu.regs[instr.Vt()])); // r8 <- *vt
		x86Asm.mov(x86::r9b, e); // r9b <- e
		if constexpr (lwc2 == OpLwc2Funct::LQV) x86Asm.call((uint64)&helperLQV);
		else if constexpr (swc2 == OpSwc2Funct::SQV) x86Asm.call((uint64)&helperSQV);
		else if constexpr (lwc2 == OpLwc2Funct::LHV) x86Asm.call((uint64)&helperLHV);
		else if constexpr (swc2 == OpSwc2Funct::SHV) x86Asm.call((uint64)&helperSHV);
		else if constexpr (lwc2 == OpLwc2Funct::LRV) x86Asm.call((uint64)&helperLRV);
		else if constexpr (swc2 == OpSwc2Funct::SRV) x86Asm.call((uint64)&helperSRV);
		else static_assert(AlwaysFalseValue<OpLwc2Funct, swc2>);

		return DecodedToken::Continue;
	}

	template <OpCop2VecFunct funct>
	N64_ABI static void helperCP2_arithmetic(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; ++i)
		{
			if constexpr (funct == OpCop2VecFunct::VADD)
			{
				const sint32 result = vs.sE[i] + vte.sE[i] + (vu.vcO.l.sE[i] != 0);
				vu.accum.l.uE[i] = result;
				vd.uE[i] = static_cast<uint16>(SignedClamp(result));
				vu.vcO.l.uE[i] = 0;
				vu.vcO.h.uE[i] = 0;
			}
			else if constexpr (funct == OpCop2VecFunct::VADDC)
			{
				const uint32 result = vs.uE[i] + vte.uE[i];
				vu.accum.l.uE[i] = result & 0xFFFF;
				vd.uE[i] = result & 0xFFFF;
				vu.vcO.l.uE[i] = VuFlag16((result >> 16) & 1);
				vu.vcO.h.uE[i] = VuFlag16(0);
			}
			else if constexpr (funct == OpCop2VecFunct::VAND)
			{
				const uint16 result = vte.uE[i] & vs.uE[i];
				vd.uE[i] = result;
				vu.accum.l.uE[i] = result;
			}
			else if constexpr (funct == OpCop2VecFunct::VSUB)
			{
				const sint32 result = vs.sE[i] - vte.sE[i] - (vu.vcO.l.uE[i] != 0);
				vu.accum.l.sE[i] = result;
				vd.sE[i] = SignedClamp(result);
				vu.vcO.l.uE[i] = VuFlag16(0);
				vu.vcO.h.uE[i] = VuFlag16(0);
			}
			else if constexpr (funct == OpCop2VecFunct::VSUBC)
			{
				const uint32 result = vs.uE[i] - vte.uE[i];
				const uint16 hresult = result & 0xFFFF;
				const bool carry = (result >> 16) & 1;
				vd.uE[i] = hresult;
				vu.accum.l.uE[i] = hresult;
				vu.vcO.l.uE[i] = VuFlag16(carry);
				vu.vcO.h.uE[i] = VuFlag16(result != 0);
			}
			else if constexpr (funct == OpCop2VecFunct::VXOR)
			{
				vu.accum.l.uE[i] = vte.uE[i] ^ vs.uE[i];
				vd.uE[i] = vu.accum.l.uE[i];
			}
			else if constexpr (funct == OpCop2VecFunct::Undocumented_0xFE)
			{
				vu.accum.l.uE[i] = vte.uE[i] + vs.uE[i];
				vd.single = 0;
			}
			else static_assert(AlwaysFalseValue<OpCop2VecFunct, funct>);
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L1235
	N64_ABI static void helperVMUDH(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			sint64 accum = (sint64)product;
			const sint16 result = SignedClamp(accum);
			accum <<= 16;
			SetAccum48(vu, e, accum);
			vd.uE[e] = result;
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L1235
	N64_ABI static void helperVMUDL(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const uint64 multiplicand1 = vte.uE[e];
			const uint64 multiplicand2 = vs.uE[e];
			const uint64 product = multiplicand1 * multiplicand2;
			uint64 accum = product >> 16;
			SetAccum48(vu, e, accum);
			uint16 result;
			if (IsSignExtension(vu.accum.h.sE[e], vu.accum.m.sE[e]))
			{
				result = vu.accum.l.uE[e];
			}
			else if (vu.accum.h.sE[e] < 0)
			{
				result = 0;
			}
			else
			{
				result = 0xFFFF;
			}
			vd.uE[e] = result;
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L1235
	N64_ABI static void helperVMUDM(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const uint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			const sint64 accum = product;
			const sint16 result = SignedClamp(accum >> 16);
			SetAccum48(vu, e, accum);
			vd.uE[e] = result;
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L1254
	N64_ABI static void helperVMUDN(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const uint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			const sint64 accum = product;
			SetAccum48(vu, e, accum);
			uint16 result;
			if (IsSignExtension(vu.accum.h.sE[e], vu.accum.m.sE[e]))
			{
				result = vu.accum.l.uE[e];
			}
			else if (vu.accum.h.sE[e] < 0)
			{
				result = 0;
			}
			else
			{
				result = 0xFFFF;
			}
			vd.uE[e] = result;
		}
	}

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp_vector_instructions.c#L968
	N64_ABI static void helperVMADH(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			const uint32 uP = product;
			const uint64 accumDelta = static_cast<uint64>(uP) << 16;
			sint64 acc = GetAccum48(vu, e) + accumDelta;
			SetAccum48(vu, e, acc);
			acc = GetAccum48(vu, e);
			const sint16 result = SignedClamp(acc >> 16);
			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMADL(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const uint64 multiplicand1 = vte.uE[e];
			const uint64 multiplicand2 = vs.uE[e];
			const uint64 product = multiplicand1 * multiplicand2;
			const uint64 accumDelta = product >> 16;
			const uint64 accum = GetAccum48(vu, e) + accumDelta;
			SetAccum48(vu, e, accum);
			uint16 result;
			if (IsSignExtension(vu.accum.h.sE[e], vu.accum.m.sE[e]))
			{
				result = vu.accum.l.uE[e];
			}
			else if (vu.accum.h.sE[e] < 0)
			{
				result = 0;
			}
			else
			{
				result = 0xFFFF;
			}
			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMADM(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const uint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			const sint64 accumDelta = product;
			sint64 accum = GetAccum48(vu, e);
			accum += accumDelta;
			SetAccum48(vu, e, accum);
			accum = GetAccum48(vu, e);
			const sint16 result = SignedClamp(accum >> 16);
			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMADN(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const uint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;
			const sint64 accumDelta = product;
			const sint64 accum = GetAccum48(vu, e) + accumDelta;
			SetAccum48(vu, e, accum);
			uint16 result;
			if (IsSignExtension(vu.accum.h.sE[e], vu.accum.m.sE[e]))
			{
				result = vu.accum.l.uE[e];
			}
			else if (vu.accum.h.sE[e] < 0)
			{
				result = 0;
			}
			else
			{
				result = 0xFFFF;
			}
			vd.uE[e] = result;
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

	N64_ABI static void helperVRCP(Rsp& rsp, const Vpr_t& vt, uint8 e, uint16* vdE)
	{
		const uint32 input = vt.sE[VuElementIndex(e & 7)];
		const uint32 result = Rcp(input);
		*vdE = result & 0xFFFF;

		auto&& div = Process::AccessDiv(rsp);
		div.divOut = (result >> 16) & 0xFFFF;
		div.divInLoaded = false;

		auto&& vu = Process::AccessVU(rsp);
		const Vpr_t vte = GetVtE(vt, e);
		vu.accum.l.single = vte.single;
	}

	N64_ABI static void helperVRCPL(Rsp& rsp, const Vpr_t& vt, uint8 e, uint16* vdE)
	{
		auto&& div = Process::AccessDiv(rsp);
		sint32 input;
		if (div.divInLoaded)
		{
			input = (div.divIn << 16) | vt.uE[VuElementIndex(e & 7)];
		}
		else
		{
			input = vt.sE[VuElementIndex(e & 7)];
		}
		const uint32 result = Rsq(input);
		*vdE = result;

		div.divOut = result >> 16;
		div.divIn = 0;
		div.divInLoaded = false;

		auto&& vu = Process::AccessVU(rsp);
		const Vpr_t vte = GetVtE(vt, e);
		vu.accum.l.single = vte.single;
	}

	N64_ABI static void helperVRCPH(Rsp& rsp, const Vpr_t& vt, uint8 e, uint16* vdE)
	{
		auto&& div = Process::AccessDiv(rsp);
		*vdE = div.divOut;

		div.divIn = vt.uE[VuElementIndex(e & 7)];
		div.divInLoaded = true;

		auto&& vu = Process::AccessVU(rsp);
		const Vpr_t vte = GetVtE(vt, e);
		vu.accum.l.single = vte.single;
	}

	N64_ABI static void helperLQV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint32 endAddr = ((address & ~15) + 15);
		for (int i = 0; address + i <= endAddr && i + e < 16; ++i)
		{
			vt.bytes[VuByteIndex(i + e)] = dmem.ReadSpByte(address + i);
		}
	}

	N64_ABI static void helperSQV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const uint32 endAddr = ((address & ~15) + 15);
		for (int i = 0; address + i <= endAddr; ++i)
		{
			dmem.WriteSpByte(address + i, vt.bytes[VuByteIndex((i + e) & 15)]);
		}
	}

	N64_ABI static void helperLHV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint32 addr3 = address & 0x7;
		address &= ~0x7;

		for (int i = 0; i < 8; i++)
		{
			const sint32 offset = ((16 - e) + (i * 2) + addr3) & 0xF;
			uint16 value = dmem.ReadSpByte(address + offset);
			value <<= 7;
			vt.uE[VuElementIndex(i)] = value;
		}
	}

	N64_ABI static void helperSHV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const uint32 addr3 = address & 0x7;
		address &= ~0x7;

		for (int i = 0; i < 8; i++)
		{
			const uint8 byteIndex = (i * 2) + e;
			uint16 val = vt.bytes[VuByteIndex(byteIndex & 15)] << 1;
			val |= vt.bytes[VuByteIndex((byteIndex + 1) & 15)] >> 7;
			const uint8 b = val & 0xFF;

			const uint32 offset = addr3 + (i * 2);
			dmem.WriteSpByte(address + (offset & 0xF), b);
		}
	}

	N64_ABI static void helperLRV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const int start = 16 - ((address & 0xF) - e);
		address &= 0xFFFFFFF0;

		for (int i = start; i < 16; i++)
		{
			vt.bytes[VuByteIndex(i & 0xF)] = dmem.ReadSpByte(address++);
		}
	}

	N64_ABI static void helperSRV(SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const int start = e;
		const int end = start + (address & 15);
		const int base = 16 - (address & 15);
		address &= ~15;
		for (int i = start; i < end; i++)
		{
			dmem.WriteSpByte(address++, vt.bytes[VuByteIndex((i + base) & 0xF)]);
		}
	}
};
