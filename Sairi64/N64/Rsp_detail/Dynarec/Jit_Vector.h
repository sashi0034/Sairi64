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

	template <OpCopSub sub> [[nodiscard]]
	static DecodedToken MC2_template(const AssembleContext& ctx, InstructionCop2VecSub instr)
	{
		JIT_SP;
		const uint8 rt = instr.Rt();
		if constexpr (sub == OpCopSub::MFC)
		{
			if (rt == 0) return DecodedToken::Continue;
		}
		auto&& x86Asm = *ctx.x86Asm;
		auto&& rsp = *ctx.rsp;
		auto&& gpr = Process::AccessGpr(rsp);

		x86Asm.mov(x86::rcx, (uint64)&gpr[rt]); // rcx <- *rt
		x86Asm.mov(x86::rdx, (uint64)&Process::AccessVU(rsp).regs[instr.Rd()]);
		x86Asm.mov(x86::r8b, instr.Element());
		if constexpr (sub == OpCopSub::MFC)
			x86Asm.call((uint64)&helperMFC2);
		else if constexpr (sub == OpCopSub::MTC)
			x86Asm.call((uint64)&helperMTC2);
		else static_assert(AlwaysFalseValue<OpCopSub, sub>);
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
		else if constexpr (funct == OpCop2VecFunct::VMULF)
			x86Asm.call((uint64)&helperVMULF);
		else if constexpr (funct == OpCop2VecFunct::VMULU)
			x86Asm.call((uint64)&helperVMULU);
		else if constexpr (funct == OpCop2VecFunct::VMULQ)
			x86Asm.call((uint64)&helperVMULQ);
		else if constexpr (funct == OpCop2VecFunct::VMACF)
			x86Asm.call((uint64)&helperVMACF);
		else if constexpr (funct == OpCop2VecFunct::VMACU)
			x86Asm.call((uint64)&helperVMACU);
		else if constexpr (funct == OpCop2VecFunct::VABS)
			x86Asm.call((uint64)&helperVABS);
		else if constexpr (funct == OpCop2VecFunct::VLT)
			x86Asm.call((uint64)&helperVLT);
		else if constexpr (funct == OpCop2VecFunct::VEQ)
			x86Asm.call((uint64)&helperVEQ);
		else if constexpr (funct == OpCop2VecFunct::VNE)
			x86Asm.call((uint64)&helperVNE);
		else if constexpr (funct == OpCop2VecFunct::VGE)
			x86Asm.call((uint64)&helperVGE);
		else if constexpr (funct == OpCop2VecFunct::VCL)
			x86Asm.call((uint64)&helperVCL);
		else if constexpr (funct == OpCop2VecFunct::VCH)
			x86Asm.call((uint64)&helperVCH);
		else if constexpr (funct == OpCop2VecFunct::VCR)
			x86Asm.call((uint64)&helperVCR);
		else if constexpr (funct == OpCop2VecFunct::VMRG)
			x86Asm.call((uint64)&helperVMRG);
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

	N64_ABI static void helperMFC2(uint32* rt, const Vpr_t& rd, uint8 e)
	{
		const uint8 hi = rd.bytes[VuByteIndex(e)];
		const uint8 lo = rd.bytes[VuByteIndex((e + 1) & 0xF)];
		const sint32 element = static_cast<sint16>(hi << 8 | lo);
		*rt = element;
	}

	N64_ABI static void helperMTC2(const uint32* rt, Vpr_t& rd, uint8 e)
	{
		const uint16 element = *rt;
		const uint8 lo = element & 0xFF;
		const uint8 hi = (element >> 8) & 0xFF;
		rd.bytes[VuByteIndex(e + 0)] = hi;
		if (e < 0xF) rd.bytes[VuByteIndex(e + 1)] = lo;
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
		const sint32 offset = SignExtend7Bit(instr.Offset(), shiftAmount);

		if constexpr (lwc2 == OpLwc2Funct::LTV || swc2 == OpSwc2Funct::STV)
		{
			x86Asm.mov(x86::rcx, (uint64)&rsp); // rcx <- *rsp
			x86Asm.mov(x86::r8b, instr.Vt()); // r8 <- vt
		}
		else
		{
			// default
			x86Asm.mov(x86::rcx, (uint64)&rsp.Dmem()); // rcx <- *dmem
			x86Asm.mov(x86::r8, reinterpret_cast<uint64>(&vu.regs[instr.Vt()])); // r8 <- *vt
		}
		x86Asm.mov(x86::eax, x86::dword_ptr(reinterpret_cast<uint64>(&Process::AccessGpr(rsp)[instr.Base()])));
		x86Asm.add(x86::eax, offset);
		x86Asm.mov(x86::edx, x86::eax); // edx <- address
		x86Asm.mov(x86::r9b, e); // r9b <- e
		if constexpr (lwc2 == OpLwc2Funct::LQV) x86Asm.call((uint64)&helperLQV);
		else if constexpr (swc2 == OpSwc2Funct::SQV) x86Asm.call((uint64)&helperSQV);
		else if constexpr (lwc2 == OpLwc2Funct::LHV) x86Asm.call((uint64)&helperLHV);
		else if constexpr (swc2 == OpSwc2Funct::SHV) x86Asm.call((uint64)&helperSHV);
		else if constexpr (lwc2 == OpLwc2Funct::LRV) x86Asm.call((uint64)&helperLRV);
		else if constexpr (swc2 == OpSwc2Funct::SRV) x86Asm.call((uint64)&helperSRV);
		else if constexpr (lwc2 == OpLwc2Funct::LDV) x86Asm.call((uint64)&helperLDV);
		else if constexpr (swc2 == OpSwc2Funct::SDV) x86Asm.call((uint64)&helperSDV);
		else if constexpr (lwc2 == OpLwc2Funct::LLV) x86Asm.call((uint64)&helperLLV);
		else if constexpr (swc2 == OpSwc2Funct::SLV) x86Asm.call((uint64)&helperSLV);
		else if constexpr (lwc2 == OpLwc2Funct::LSV) x86Asm.call((uint64)&helperLSV);
		else if constexpr (swc2 == OpSwc2Funct::SSV) x86Asm.call((uint64)&helperSSV);
		else if constexpr (lwc2 == OpLwc2Funct::LBV) x86Asm.call((uint64)&helperLBV);
		else if constexpr (swc2 == OpSwc2Funct::SBV) x86Asm.call((uint64)&helperSBV);
		else if constexpr (lwc2 == OpLwc2Funct::LPV) x86Asm.call((uint64)&helperLPV);
		else if constexpr (swc2 == OpSwc2Funct::SPV) x86Asm.call((uint64)&helperSPV);
		else if constexpr (lwc2 == OpLwc2Funct::LUV) x86Asm.call((uint64)&helperLUV);
		else if constexpr (swc2 == OpSwc2Funct::SUV) x86Asm.call((uint64)&helperSUV);
		else if constexpr (lwc2 == OpLwc2Funct::LFV) x86Asm.call((uint64)&helperLFV);
		else if constexpr (swc2 == OpSwc2Funct::SFV) x86Asm.call((uint64)&helperSFV);
		else if constexpr (swc2 == OpSwc2Funct::SWV) x86Asm.call((uint64)&helperSWV);
		else if constexpr (lwc2 == OpLwc2Funct::LTV) x86Asm.call((uint64)&helperLTV); // 引数注意
		else if constexpr (swc2 == OpSwc2Funct::STV) x86Asm.call((uint64)&helperSTV); // 引数注意
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
				const uint16 result = vte.uE[i] ^ vs.uE[i];
				vu.accum.l.uE[i] = result;
				vd.uE[i] = vu.accum.l.uE[i];
			}
			else if constexpr (funct == OpCop2VecFunct::VNAND)
			{
				const uint16 result = ~(vte.uE[i] & vs.uE[i]);
				vd.uE[i] = result;
				vu.accum.l.uE[i] = result;
			}
			else if constexpr (funct == OpCop2VecFunct::VOR)
			{
				const uint16 result = vte.uE[i] | vs.uE[i];
				vd.uE[i] = result;
				vu.accum.l.uE[i] = result;
			}
			else if constexpr (funct == OpCop2VecFunct::VNOR)
			{
				const uint16 result = ~(vte.uE[i] | vs.uE[i]);
				vd.uE[i] = result;
				vu.accum.l.uE[i] = result;
			}
			else if constexpr (funct == OpCop2VecFunct::VNXOR)
			{
				const uint16 result = ~(vte.uE[i] ^ vs.uE[i]);
				vd.uE[i] = result;
				vu.accum.l.uE[i] = result;
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
			const uint64 accum = product >> 16;
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
			sint64 accum = GetAccum48(vu, e) + accumDelta;
			SetAccum48(vu, e, accum);
			accum = GetAccum48(vu, e);
			const sint16 result = SignedClamp(accum >> 16);
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

	N64_ABI static void helperVMULF(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;

			sint64 accum = product;
			accum = (accum * 2) + 0x8000;

			SetAccum48(vu, e, accum);

			const sint16 result = SignedClamp(accum >> 16);
			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMULU(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;

			sint64 accum = product;
			accum = (accum * 2) + 0x8000;

			const uint16 result = UnsignedClamp(accum >> 16);

			SetAccum48(vu, e, accum);
			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMULQ(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			sint32 product = vs.sE[i] * vte.sE[i];
			if (product < 0)
			{
				product += 31;
			}

			vu.accum.h.uE[i] = product >> 16;
			vu.accum.m.uE[i] = product;
			vu.accum.l.uE[i] = 0;
			vd.uE[i] = SignedClamp(product >> 1) & ~15;
		}
	}

	N64_ABI static void helperVMACF(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;

			sint64 accumDelta = product;
			accumDelta *= 2;
			sint64 accum = GetAccum48(vu, e) + accumDelta;
			SetAccum48(vu, e, accum);
			accum = GetAccum48(vu, e);

			const sint16 result = SignedClamp(accum >> 16);

			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVMACU(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int e = 0; e < 8; e++)
		{
			const sint16 multiplicand1 = vte.uE[e];
			const sint16 multiplicand2 = vs.uE[e];
			const sint32 product = multiplicand1 * multiplicand2;

			sint64 acc_delta = product;
			acc_delta *= 2;
			sint64 accum = GetAccum48(vu, e) + acc_delta;
			SetAccum48(vu, e, accum);
			accum = GetAccum48(vu, e);

			const uint16 result = UnsignedClamp(accum >> 16);

			vd.uE[e] = result;
		}
	}

	N64_ABI static void helperVABS(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			if (vs.sE[i] < 0)
			{
				if ((vte.uE[i] == 0x8000))
				[[unlikely]]
				{
					vd.uE[i] = 0x7FFF;
					vu.accum.l.uE[i] = 0x8000;
				}
				else
				{
					vd.uE[i] = -vte.sE[i];
					vu.accum.l.uE[i] = -vte.sE[i];
				}
			}
			else if (vs.uE[i] == 0)
			{
				vd.uE[i] = 0x0000;
				vu.accum.l.uE[i] = 0x0000;
			}
			else
			{
				vd.uE[i] = vte.uE[i];
				vu.accum.l.uE[i] = vte.uE[i];
			}
		}
	}

	N64_ABI static void helperVLT(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			const bool eq = vs.uE[i] == vte.uE[i];
			const bool negative = vu.vcO.h.uE[i] != 0 && vu.vcO.l.uE[i] != 0 && eq;
			vu.vcC.l.uE[i] = VuFlag16(negative || (vs.sE[i] < vte.sE[i]));
			vu.accum.l.uE[i] = vu.vcC.l.uE[i] != 0 ? vs.uE[i] : vte.uE[i];
			vd.uE[i] = vu.accum.l.uE[i];

			vu.vcC.h.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
			vu.vcO.l.uE[i] = VuFlag16(0);
		}
	}

	N64_ABI static void helperVEQ(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			vu.vcC.l.uE[i] = VuFlag16((vu.vcO.h.uE[i] == 0) && (vs.uE[i] == vte.uE[i]));
			vu.accum.l.uE[i] = vu.vcC.l.uE[i] != 0 ? vs.uE[i] : vte.uE[i];
			vd.uE[i] = vu.accum.l.uE[i];

			vu.vcC.h.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
			vu.vcO.l.uE[i] = VuFlag16(0);
		}
	}

	N64_ABI static void helperVNE(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			vu.vcC.l.uE[i] = VuFlag16((vu.vcO.h.uE[i] != 0) || (vs.uE[i] != vte.uE[i]));
			vu.accum.l.uE[i] = vu.vcC.l.uE[i] != 0 ? vs.uE[i] : vte.uE[i];
			vd.uE[i] = vu.accum.l.uE[i];

			vu.vcC.h.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
			vu.vcO.l.uE[i] = VuFlag16(0);
		}
	}

	N64_ABI static void helperVGE(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			const bool eql = vs.sE[i] == vte.sE[i];
			const bool neg = !(vu.vcO.l.uE[i] != 0 && vu.vcO.h.uE[i] != 0) && eql;

			vu.vcC.l.uE[i] = VuFlag16(neg || (vs.sE[i] > vte.sE[i]));
			vu.accum.l.uE[i] = vu.vcC.l.uE[i] != 0 ? vs.uE[i] : vte.uE[i];
			vd.uE[i] = vu.accum.l.uE[i];
			vu.vcC.h.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
			vu.vcO.l.uE[i] = VuFlag16(0);
		}
	}

	N64_ABI static void helperVCL(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			uint16 vsElement = vs.uE[i];
			uint16 vteElement = vte.uE[i];

			if (vu.vcO.l.uE[i])
			{
				if (vu.vcO.h.uE[i])
				{
					vu.accum.l.uE[i] = vu.vcC.l.uE[i] ? -vteElement : vsElement;
				}
				else
				{
					const uint16 clampedSum = vsElement + vteElement;
					const bool overflow = (vsElement + vteElement) != clampedSum;
					if (vu.vcE.uE[i])
					{
						vu.vcC.l.uE[i] = VuFlag16(!clampedSum || !overflow);
						vu.accum.l.uE[i] = vu.vcC.l.uE[i] ? -vteElement : vsElement;
					}
					else
					{
						vu.vcC.l.uE[i] = VuFlag16(!clampedSum && !overflow);
						vu.accum.l.uE[i] = vu.vcC.l.uE[i] ? -vteElement : vsElement;
					}
				}
			}
			else
			{
				if (vu.vcO.h.uE[i])
				{
					vu.accum.l.uE[i] = vu.vcC.h.uE[i] ? vteElement : vsElement;
				}
				else
				{
					vu.vcC.h.uE[i] = VuFlag16(static_cast<sint32>(vsElement) - static_cast<sint32>(vteElement) >= 0);
					vu.accum.l.uE[i] = vu.vcC.h.uE[i] ? vteElement : vsElement;
				}
			}
		}

		memset(&vu.vcO.l, 0, sizeof(Vpr_t));
		memset(&vu.vcO.h, 0, sizeof(Vpr_t));
		memset(&vu.vcE, 0, sizeof(Vpr_t));
		memcpy(&vd, &vu.accum.l, sizeof(Vpr_t));
	}

	N64_ABI static void helperVCH(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			const sint16 vsElement = vs.sE[i];
			const sint16 vteElement = vte.sE[i];

			if ((vsElement ^ vteElement) < 0)
			{
				const sint16 result = vsElement + vteElement;

				vu.accum.l.sE[i] = (result <= 0 ? -vteElement : vsElement);
				vu.vcC.l.uE[i] = VuFlag16(result <= 0);
				vu.vcC.h.uE[i] = VuFlag16(vteElement < 0);
				vu.vcO.l.uE[i] = VuFlag16(1);
				vu.vcO.h.uE[i] = VuFlag16(
					result != 0 && static_cast<uint16>(vsElement) != (static_cast<uint16>(vteElement) ^ 0xFFFF));
				vu.vcE.uE[i] = VuFlag16(result == -1);
			}
			else
			{
				const sint16 result = vsElement - vteElement;

				vu.accum.l.uE[i] = (result >= 0 ? vteElement : vsElement);
				vu.vcC.l.uE[i] = VuFlag16(vteElement < 0);
				vu.vcC.h.uE[i] = VuFlag16(result >= 0);
				vu.vcO.l.uE[i] = VuFlag16(0);
				vu.vcO.h.uE[i] = VuFlag16(
					result != 0 && static_cast<uint16>(vsElement) != (static_cast<uint16>(vteElement) ^ 0xFFFF));
				vu.vcE.uE[i] = VuFlag16(0);
			}

			vd.uE[i] = vu.accum.l.uE[i];
		}
	}

	N64_ABI static void helperVCR(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			const uint16 vsElement = vs.uE[i];
			const uint16 vteElement = vte.uE[i];

			const bool signDifferent = (0x8000 & (vsElement ^ vteElement)) == 0x8000;

			const uint16 vtAbs = signDifferent ? ~vteElement : vteElement;

			const bool gte = static_cast<sint16>(vteElement) <= static_cast<sint16>(signDifferent ? 0xFFFF : vsElement);
			const bool lte = (((signDifferent ? vsElement : 0) + vteElement) & 0x8000) == 0x8000;

			const bool check = signDifferent ? lte : gte;
			const uint16 result = check ? vtAbs : vsElement;

			vu.accum.l.uE[i] = result;
			vd.uE[i] = result;

			vu.vcC.h.uE[i] = VuFlag16(gte);
			vu.vcC.l.uE[i] = VuFlag16(lte);

			vu.vcO.l.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
			vu.vcE.uE[i] = VuFlag16(0);
		}
	}

	N64_ABI static void helperVMRG(VU& vu, Vpr_t& vd, const Vpr_t& vs, const Vpr_t& vt, uint8 element)
	{
		const Vpr_t vte = GetVtE(vt, element);
		for (int i = 0; i < 8; i++)
		{
			vu.accum.l.uE[i] = vu.vcC.l.uE[i] != 0 ? vs.uE[i] : vte.uE[i];
			vd.uE[i] = vu.accum.l.uE[i];

			vu.vcO.l.uE[i] = VuFlag16(0);
			vu.vcO.h.uE[i] = VuFlag16(0);
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

	N64_ABI static void helperLDV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const int start = e;
		const int end = std::min(start + 8, 16);
		for (int i = start; i < end; i++)
		{
			vt.bytes[VuByteIndex(i)] = dmem.ReadSpByte(address);
			address++;
		}
	}

	N64_ABI static void helperSDV(SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		for (int i = 0; i < 8; i++)
		{
			const int element = i + e;
			dmem.WriteSpByte(address + i, vt.bytes[VuByteIndex(element & 0xF)]);
		}
	}

	N64_ABI static void helperLLV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		for (int i = 0; i < 4; i++)
		{
			const int element = i + e;
			if (element > 15) break;
			vt.bytes[VuByteIndex(element)] = dmem.ReadSpByte(address + i);
		}
	}

	N64_ABI static void helperSLV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		for (int i = 0; i < 4; i++)
		{
			const int element = i + e;
			dmem.WriteSpByte(address + i, vt.bytes[VuByteIndex(element & 0xF)]);
		}
	}

	N64_ABI static void helperLSV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint16 value = dmem.ReadSpHalf(address);
		const uint8 lo = value & 0xFF;
		const uint8 hi = (value >> 8) & 0xFF;
		vt.bytes[VuByteIndex(e + 0)] = hi;
		if (e < 15)
		{
			vt.bytes[VuByteIndex(e + 1)] = lo;
		}
	}

	N64_ABI static void helperSSV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const uint8 hi = vt.bytes[VuByteIndex((e + 0) & 15)];
		const uint8 lo = vt.bytes[VuByteIndex((e + 1) & 15)];
		const uint16 value = static_cast<uint16>(hi) << 8 | lo;

		dmem.WriteSpHalf(address, value);
	}

	N64_ABI static void helperLBV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		vt.bytes[VuByteIndex(e)] = dmem.ReadSpByte(address);
	}

	N64_ABI static void helperSBV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		dmem.WriteSpByte(address, vt.bytes[VuByteIndex(e)]);
	}

	N64_ABI static void helperLPV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint32 addressOffset = address & 7;
		address &= ~7;

		for (int i = 0; i < 8; i++)
		{
			const int elementOffset = (16 - e + (i + addressOffset)) & 0xF;

			uint16 value = dmem.ReadSpByte(address + elementOffset);
			value <<= 8;
			vt.uE[VuElementIndex(i)] = value;
		}
	}

	N64_ABI static void helperSPV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const uint8 start = e;
		const uint8 end = start + 8;

		for (int offset = start; offset < end; offset++)
		{
			if ((offset & 15) < 8)
			{
				dmem.WriteSpByte(address++, vt.bytes[VuByteIndex((offset & 7) << 1)]);
			}
			else
			{
				dmem.WriteSpByte(address++, vt.uE[VuElementIndex(offset & 7)] >> 7);
			}
		}
	}

	N64_ABI static void helperLUV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint32 addressOffset = address & 7;
		address &= ~7;

		for (int i = 0; i < 8; i++)
		{
			const int elementOffset = (16 - e + (i + addressOffset)) & 0xF;

			uint16 value = dmem.ReadSpByte(address + elementOffset);
			value <<= 7;
			vt.uE[VuElementIndex(i)] = value;
		}
	}

	N64_ABI static void helperSUV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const int start = e;
		const int end = start + 8;
		for (int offset = start; offset < end; offset++)
		{
			if ((offset & 15) < 8)
			{
				dmem.WriteSpByte(address++, vt.uE[VuElementIndex(offset & 7)] >> 7);
			}
			else
			{
				dmem.WriteSpByte(address++, vt.bytes[VuByteIndex((offset & 7) << 1)]);
			}
		}
	}

	N64_ABI static void helperLFV(const SpDmem& dmem, uint32 address, Vpr_t& vt, uint8 e)
	{
		const uint32 base = (address & 7) - e;
		address &= ~7;

		const int start = e;
		const int end = std::min(start + 8, 16);

		Vpr_t temp;
		for (uint32 offset = 0; offset < 4; offset++)
		{
			temp.uE[VuElementIndex(offset + 0)] = dmem.ReadSpByte(address + (base + offset * 4 + 0 & 15)) << 7;
			temp.uE[VuElementIndex(offset + 4)] = dmem.ReadSpByte(address + (base + offset * 4 + 8 & 15)) << 7;
		}

		for (uint32 offset = start; offset < end; offset++)
		{
			vt.bytes[VuByteIndex(offset)] = temp.bytes[VuByteIndex(offset)];
		}
	}

	N64_ABI static void helperSFV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		const uint32 base = address & 7;
		address &= ~7;

		uint8 values[4] = {0, 0, 0, 0};

		switch (e)
		{
		case 0:
		case 15:
			values[0] = vt.uE[VuElementIndex(0)] >> 7;
			values[1] = vt.uE[VuElementIndex(1)] >> 7;
			values[2] = vt.uE[VuElementIndex(2)] >> 7;
			values[3] = vt.uE[VuElementIndex(3)] >> 7;
			break;
		case 1:
			values[0] = vt.uE[VuElementIndex(6)] >> 7;
			values[1] = vt.uE[VuElementIndex(7)] >> 7;
			values[2] = vt.uE[VuElementIndex(4)] >> 7;
			values[3] = vt.uE[VuElementIndex(5)] >> 7;
			break;
		case 4:
			values[0] = vt.uE[VuElementIndex(1)] >> 7;
			values[1] = vt.uE[VuElementIndex(2)] >> 7;
			values[2] = vt.uE[VuElementIndex(3)] >> 7;
			values[3] = vt.uE[VuElementIndex(0)] >> 7;
			break;
		case 5:
			values[0] = vt.uE[VuElementIndex(7)] >> 7;
			values[1] = vt.uE[VuElementIndex(4)] >> 7;
			values[2] = vt.uE[VuElementIndex(5)] >> 7;
			values[3] = vt.uE[VuElementIndex(6)] >> 7;
			break;
		case 8:
			values[0] = vt.uE[VuElementIndex(4)] >> 7;
			values[1] = vt.uE[VuElementIndex(5)] >> 7;
			values[2] = vt.uE[VuElementIndex(6)] >> 7;
			values[3] = vt.uE[VuElementIndex(7)] >> 7;
			break;
		case 11:
			values[0] = vt.uE[VuElementIndex(3)] >> 7;
			values[1] = vt.uE[VuElementIndex(0)] >> 7;
			values[2] = vt.uE[VuElementIndex(1)] >> 7;
			values[3] = vt.uE[VuElementIndex(2)] >> 7;
			break;
		case 12:
			values[0] = vt.uE[VuElementIndex(5)] >> 7;
			values[1] = vt.uE[VuElementIndex(6)] >> 7;
			values[2] = vt.uE[VuElementIndex(7)] >> 7;
			values[3] = vt.uE[VuElementIndex(4)] >> 7;
			break;
		default:
			break;
		}

		for (int i = 0; i < 4; i++)
		{
			dmem.WriteSpByte(address + ((base + (i << 2)) & 15), values[i]);
		}
	}

	N64_ABI static void helperSWV(SpDmem& dmem, uint32 address, const Vpr_t& vt, uint8 e)
	{
		uint32 base = address & 7;
		address &= ~7;

		for (int i = e; i < e + 16; i++)
		{
			dmem.WriteSpByte(address + (base & 15), vt.bytes[VuByteIndex(i & 15)]);
			base++;
		}
	}

	N64_ABI static void helperLTV(Rsp& rsp, uint32 address, uint8 vt, uint8 e)
	{
		auto&& dmem = rsp.Dmem();
		auto&& vu = Process::AccessVU(rsp);

		uint32 base = address;
		base &= ~7;

		for (int i = 0; i < 8; i++)
		{
			const uint32 offset = (i * 2) + e + ((base) & 8);

			const uint16 hi = dmem.ReadSpByte(base + ((offset + 0) & 0xF));
			const uint16 lo = dmem.ReadSpByte(base + ((offset + 1) & 0xF));

			const int reg = (vt & 0x18) | ((i + (e >> 1)) & 0x7);
			vu.regs[reg].uE[VuElementIndex(i & 0x7)] = (hi << 8) | lo;
		}
	}

	N64_ABI static void helperSTV(Rsp& rsp, uint32 address, uint8 vt, uint8 e)
	{
		auto&& dmem = rsp.Dmem();
		auto&& vu = Process::AccessVU(rsp);

		uint32 base = address;
		const uint32 addressOffset = base & 0x7;
		base &= ~0x7;

		e = e >> 1;

		for (int i = 0; i < 8; i++)
		{
			const uint32 offset = (i * 2) + addressOffset;

			const int reg = (vt & 0x18) | ((i + e) & 0x7);

			const uint16 value = vu.regs[reg].uE[VuElementIndex(i & 0x7)];
			const uint16 hi = (value >> 8) & 0xFF;
			const uint16 lo = (value >> 0) & 0xFF;

			dmem.WriteSpByte(base + ((offset + 0) & 0xF), hi);
			dmem.WriteSpByte(base + ((offset + 1) & 0xF), lo);
		}
	}
};
