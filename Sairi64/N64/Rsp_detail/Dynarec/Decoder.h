﻿#pragma once

#include "Jit_Vector.h"
#include "N64/Instruction.h"

namespace N64::Rsp_detail::Dynarec
{
	class Decoder;
}

class N64::Rsp_detail::Dynarec::Decoder
{
public:
	static DecodedToken AssembleInstr(const AssembleContext& ctx, const AssembleState& state, Instruction instr)
	{
		switch (instr.Op())
		{
		case Opcode::SPECIAL:
			break;
		case Opcode::REGIMM:
			break;
		case Opcode::J:
			break;
		case Opcode::JAL:
			break;
		case Opcode::BEQ:
			break;
		case Opcode::BNE:
			break;
		case Opcode::BLEZ:
			break;
		case Opcode::BGTZ:
			break;
		case Opcode::ADDI:
			return Jit::I_immediateArithmetic<Opcode::ADDI>(ctx, static_cast<InstructionI>(instr));
		case Opcode::ADDIU:
			return Jit::I_immediateArithmetic<Opcode::ADDIU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SLTI:
			break;
		case Opcode::SLTIU:
			break;
		case Opcode::ANDI:
			return Jit::I_immediateArithmetic<Opcode::ANDI>(ctx, static_cast<InstructionI>(instr));
		case Opcode::ORI:
			return Jit::I_immediateArithmetic<Opcode::ORI>(ctx, static_cast<InstructionI>(instr));
		case Opcode::XORI:
			return Jit::I_immediateArithmetic<Opcode::XORI>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LUI:
			return Jit::LUI(ctx, static_cast<InstructionI>(instr));
		case Opcode::CP0:
			break;
		case Opcode::CP1:
			break;
		case Opcode::CP2:
			return assembleCP2(ctx, state, static_cast<InstructionCop2Vec>(instr));
		case Opcode::CP3:
			break;
		case Opcode::BEQL:
			break;
		case Opcode::BNEL:
			break;
		case Opcode::BLEZL:
			break;
		case Opcode::BGTZL:
			break;
		case Opcode::DADDI:
			break;
		case Opcode::DADDIU:
			break;
		case Opcode::LDL:
			break;
		case Opcode::LDR:
			break;
		case Opcode::RDHWR:
			break;
		case Opcode::LB:
			break;
		case Opcode::LH:
			break;
		case Opcode::LWL:
			break;
		case Opcode::LW:
			return Jit::L_load<Opcode::LW>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LBU:
			break;
		case Opcode::LHU:
			break;
		case Opcode::LWR:
			break;
		case Opcode::LWU:
			return Jit::L_load<Opcode::LWU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SB:
			break;
		case Opcode::SH:
			break;
		case Opcode::SWL:
			break;
		case Opcode::SW:
			break;
		case Opcode::SDL:
			break;
		case Opcode::SDR:
			break;
		case Opcode::SWR:
			break;
		case Opcode::CACHE:
			break;
		case Opcode::LL:
			break;
		case Opcode::LWC2:
			return assembleLWC2(ctx, state, static_cast<InstructionV>(instr));
		case Opcode::LLD:
			break;
		case Opcode::LDC1:
			break;
		case Opcode::LD:
			break;
		case Opcode::SC:
			break;
		case Opcode::SWC2:
			break;
		case Opcode::SCD:
			break;
		case Opcode::SDC1:
			break;
		case Opcode::SD:
			break;
		case Opcode::Invalid_0xFF:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

private:
	static DecodedToken assembleCP2(const AssembleContext& ctx, const AssembleState& state, InstructionCop2Vec instr)
	{
		if (instr.IsFunct())
			return assembleCP2_funct(ctx, state, static_cast<InstructionCop2VecFunct>(instr));
		else
			return assembleCP2_sub(ctx, state, static_cast<InstructionCop2VecSub>(instr));
	}

	static DecodedToken assembleCP2_funct(
		const AssembleContext& ctx, const AssembleState& state, InstructionCop2VecFunct instr)
	{
		switch (instr.Funct())
		{
		case OpCop2VecFunct::VMULF:
			break;
		case OpCop2VecFunct::VMULU:
			break;
		case OpCop2VecFunct::VRNDP:
			break;
		case OpCop2VecFunct::VMULQ:
			break;
		case OpCop2VecFunct::VMUDL:
			break;
		case OpCop2VecFunct::VMUDM:
			break;
		case OpCop2VecFunct::VMUDN:
			break;
		case OpCop2VecFunct::VMUDH:
			break;
		case OpCop2VecFunct::VMACF:
			break;
		case OpCop2VecFunct::VMACU:
			break;
		case OpCop2VecFunct::VRNDN:
			break;
		case OpCop2VecFunct::VMACQ:
			break;
		case OpCop2VecFunct::VMADL:
			break;
		case OpCop2VecFunct::VMADM:
			break;
		case OpCop2VecFunct::VMADN:
			break;
		case OpCop2VecFunct::VMADH:
			break;
		case OpCop2VecFunct::VADD:
			break;
		case OpCop2VecFunct::VSUB:
			break;
		case OpCop2VecFunct::VABS:
			break;
		case OpCop2VecFunct::VADDC:
			break;
		case OpCop2VecFunct::VSUBC:
			break;
		case OpCop2VecFunct::VSAR:
			break;
		case OpCop2VecFunct::VLT:
			break;
		case OpCop2VecFunct::VEQ:
			break;
		case OpCop2VecFunct::VNE:
			break;
		case OpCop2VecFunct::VGE:
			break;
		case OpCop2VecFunct::VCL:
			break;
		case OpCop2VecFunct::VCH:
			break;
		case OpCop2VecFunct::VCR:
			break;
		case OpCop2VecFunct::VMRG:
			break;
		case OpCop2VecFunct::VAND:
			break;
		case OpCop2VecFunct::VNAND:
			break;
		case OpCop2VecFunct::VOR:
			break;
		case OpCop2VecFunct::VNOR:
			break;
		case OpCop2VecFunct::VXOR:
			return Jit::Vector::VXOR(ctx, instr);
		case OpCop2VecFunct::VNXOR:
			break;
		case OpCop2VecFunct::VRCP:
			break;
		case OpCop2VecFunct::VRCPL:
			break;
		case OpCop2VecFunct::VRCPH:
			break;
		case OpCop2VecFunct::VMOV:
			break;
		case OpCop2VecFunct::VRSQ:
			break;
		case OpCop2VecFunct::VRSQL:
			break;
		case OpCop2VecFunct::VRSQH:
			break;
		case OpCop2VecFunct::VNOP:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleCP2_sub(
		const AssembleContext& ctx, const AssembleState& state, InstructionCop2VecSub instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MFC:
			break;
		case OpCopSub::MTC:
			break;
		case OpCopSub::CFC:
			break;
		case OpCopSub::CTC:
			return Jit::Vector::CTC2(ctx, instr);
		default: break;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleLWC2(
		const AssembleContext& ctx, const AssembleState& state, InstructionV instr)
	{
		switch (instr.Funct())
		{
		case OpLwc2Funct::LBV:
			break;
		case OpLwc2Funct::LSV:
			break;
		case OpLwc2Funct::LLV:
			break;
		case OpLwc2Funct::LDV:
			break;
		case OpLwc2Funct::LQV:
			break;
		case OpLwc2Funct::LRV:
			break;
		case OpLwc2Funct::LPV:
			break;
		case OpLwc2Funct::LUV:
			break;
		case OpLwc2Funct::LHV:
			break;
		case OpLwc2Funct::LFV:
			break;
		case OpLwc2Funct::LTV:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}
};
