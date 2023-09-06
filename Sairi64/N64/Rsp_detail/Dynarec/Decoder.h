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
			return assembleSPECIAL(ctx, state, static_cast<InstructionR>(instr));
		case Opcode::REGIMM:
			break;
		case Opcode::J:
			return Jit::J_template<Opcode::J>(ctx, static_cast<InstructionJ>(instr));
		case Opcode::JAL:
			return Jit::J_template<Opcode::JAL>(ctx, static_cast<InstructionJ>(instr));
		case Opcode::BEQ:
			break;
		case Opcode::BNE:
			return Jit::B_branchOffset<Opcode::BNE>(ctx, static_cast<InstructionI>(instr));
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
			return assembleCP0(ctx, state, static_cast<InstructionCop>(instr));
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
			return Jit::S_store<Opcode::SW>(ctx, static_cast<InstructionI>(instr));
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
			return assembleLWC2(ctx, state, static_cast<InstructionLv>(instr));
		case Opcode::LLD:
			break;
		case Opcode::LDC1:
			break;
		case Opcode::LD:
			break;
		case Opcode::SC:
			break;
		case Opcode::SWC2:
			return assembleSWC2(ctx, state, static_cast<InstructionSv>(instr));
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
	static DecodedToken assembleSPECIAL(const AssembleContext& ctx, const AssembleState& state, InstructionR instr)
	{
		switch (instr.Funct())
		{
		case OpSpecialFunct::SLL:
			break;
		case OpSpecialFunct::SRL:
			break;
		case OpSpecialFunct::SRA:
			break;
		case OpSpecialFunct::SLLV:
			break;
		case OpSpecialFunct::SRLV:
			break;
		case OpSpecialFunct::SRAV:
			break;
		case OpSpecialFunct::JR:
			break;
		case OpSpecialFunct::JALR:
			break;
		case OpSpecialFunct::BREAK:
			return Jit::BREAK(ctx, instr);
		case OpSpecialFunct::SYNC:
			break;
		case OpSpecialFunct::MFHI:
			break;
		case OpSpecialFunct::MTHI:
			break;
		case OpSpecialFunct::MFLO:
			break;
		case OpSpecialFunct::MTLO:
			break;
		case OpSpecialFunct::DSLLV:
			break;
		case OpSpecialFunct::DSRLV:
			break;
		case OpSpecialFunct::DSRAV:
			break;
		case OpSpecialFunct::MULT:
			break;
		case OpSpecialFunct::MULTU:
			break;
		case OpSpecialFunct::DIV:
			break;
		case OpSpecialFunct::DIVU:
			break;
		case OpSpecialFunct::ADD:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::ADD>(ctx, instr);
		case OpSpecialFunct::ADDU:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::ADDU>(ctx, instr);
		case OpSpecialFunct::SUB:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::SUB>(ctx, instr);
		case OpSpecialFunct::SUBU:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::SUBU>(ctx, instr);
		case OpSpecialFunct::AND:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::AND>(ctx, instr);
		case OpSpecialFunct::OR:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::OR>(ctx, instr);
		case OpSpecialFunct::XOR:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::XOR>(ctx, instr);
		case OpSpecialFunct::NOR:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::NOR>(ctx, instr);
		case OpSpecialFunct::SLT:
			break;
		case OpSpecialFunct::SLTU:
			break;
		case OpSpecialFunct::TGE:
			break;
		case OpSpecialFunct::TGEU:
			break;
		case OpSpecialFunct::TLT:
			break;
		case OpSpecialFunct::TLTU:
			break;
		case OpSpecialFunct::TEQ:
			break;
		case OpSpecialFunct::TNE:
			break;
		case OpSpecialFunct::DSLL:
			break;
		case OpSpecialFunct::DSRL:
			break;
		case OpSpecialFunct::DSRA:
			break;
		case OpSpecialFunct::DSLL32:
			break;
		case OpSpecialFunct::DSRL32:
			break;
		case OpSpecialFunct::DSRA32:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleCP0(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MTC:
			return Jit::MTC0(ctx, static_cast<InstructionR>(instr));
		case OpCopSub::MFC:
			return Jit::MFC0(ctx, static_cast<InstructionR>(instr));
		default: break;
		}

		return AssumeNotImplemented(ctx, instr);
	}

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
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VADD>(ctx, instr);
		case OpCop2VecFunct::VSUB:
			break;
		case OpCop2VecFunct::VABS:
			break;
		case OpCop2VecFunct::VADDC:
			break;
		case OpCop2VecFunct::VSUBC:
			break;
		case OpCop2VecFunct::VSAR:
			return Jit::Vector::VSAR(ctx, instr);
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
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VXOR>(ctx, instr);
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
			return Jit::Vector::CFC2(ctx, instr);
		case OpCopSub::CTC:
			return Jit::Vector::CTC2(ctx, instr);
		default: break;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleLWC2(
		const AssembleContext& ctx, const AssembleState& state, InstructionLv instr)
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
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LQV>(ctx, instr);
		case OpLwc2Funct::LRV:
			break;
		case OpLwc2Funct::LPV:
			break;
		case OpLwc2Funct::LUV:
			break;
		case OpLwc2Funct::LHV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LHV>(ctx, instr);
		case OpLwc2Funct::LFV:
			break;
		case OpLwc2Funct::LTV:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleSWC2(
		const AssembleContext& ctx, const AssembleState& state, InstructionSv instr)
	{
		switch (instr.Funct())
		{
		case OpSwc2Funct::SBV:
			break;
		case OpSwc2Funct::SSV:
			break;
		case OpSwc2Funct::SLV:
			break;
		case OpSwc2Funct::SDV:
			break;
		case OpSwc2Funct::SQV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SQV>(ctx, instr);
		case OpSwc2Funct::SRV:
			break;
		case OpSwc2Funct::SPV:
			break;
		case OpSwc2Funct::SUV:
			break;
		case OpSwc2Funct::SHV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SHV>(ctx, instr);
		case OpSwc2Funct::SFV:
			break;
		case OpSwc2Funct::SWV:
			break;
		case OpSwc2Funct::STV:
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}
};
