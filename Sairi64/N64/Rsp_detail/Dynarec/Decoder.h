#pragma once

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
			return assembleREGIMM(ctx, state, static_cast<InstructionRegimm>(instr));
		case Opcode::J:
			return Jit::J_template<Opcode::J>(ctx, static_cast<InstructionJ>(instr));
		case Opcode::JAL:
			return Jit::J_template<Opcode::JAL>(ctx, static_cast<InstructionJ>(instr));
		case Opcode::BEQ:
			return Jit::B_branchOffset<Opcode::BEQ>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BNE:
			return Jit::B_branchOffset<Opcode::BNE>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BLEZ:
			return Jit::B_branchOffset<Opcode::BLEZ>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BGTZ:
			return Jit::B_branchOffset<Opcode::BGTZ>(ctx, static_cast<InstructionI>(instr));
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
			return Jit::L_load<Opcode::LB>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LH:
			return Jit::L_load<Opcode::LH>(ctx, static_cast<InstructionI>(instr));
		// case Opcode::LWL:
		// 	break;
		case Opcode::LW:
			return Jit::L_load<Opcode::LW>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LBU:
			return Jit::L_load<Opcode::LBU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LHU:
			return Jit::L_load<Opcode::LHU>(ctx, static_cast<InstructionI>(instr));
		// case Opcode::LWR:
		// 	break;
		case Opcode::LWU:
			return Jit::L_load<Opcode::LWU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SB:
			return Jit::S_store<Opcode::SB>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SH:
			return Jit::S_store<Opcode::SH>(ctx, static_cast<InstructionI>(instr));
		// case Opcode::SWL:
		// 	break;
		case Opcode::SW:
			return Jit::S_store<Opcode::SW>(ctx, static_cast<InstructionI>(instr));
		// case Opcode::SDL:
		// 	break;
		// case Opcode::SDR:
		// 	break;
		// case Opcode::SWR:
		// 	break;
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
			return Jit::SPECIAL_shift<OpSpecialFunct::SLL>(ctx, instr);
		case OpSpecialFunct::SRL:
			return Jit::SPECIAL_shift<OpSpecialFunct::SRL>(ctx, instr);
		case OpSpecialFunct::SRA:
			return Jit::SPECIAL_shift<OpSpecialFunct::SRA>(ctx, instr);
		case OpSpecialFunct::SLLV:
			return Jit::SPECIAL_shiftVariable<OpSpecialFunct::SLLV>(ctx, instr);
		case OpSpecialFunct::SRLV:
			return Jit::SPECIAL_shiftVariable<OpSpecialFunct::SRLV>(ctx, instr);
		case OpSpecialFunct::SRAV:
			return Jit::SPECIAL_shiftVariable<OpSpecialFunct::SRAV>(ctx, instr);
		case OpSpecialFunct::JR:
			return Jit::JR_template<OpSpecialFunct::JR>(ctx, instr);
		case OpSpecialFunct::JALR:
			return Jit::JR_template<OpSpecialFunct::JALR>(ctx, instr);
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
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleCP0(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MTC:
			return Jit::MTC0(ctx, static_cast<InstructionCopSub>(instr));
		case OpCopSub::MFC:
			return Jit::MFC0(ctx, static_cast<InstructionCopSub>(instr));
		default: break;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleREGIMM(const AssembleContext& ctx, const AssembleState& state, InstructionRegimm instr)
	{
		switch (instr.Sub())
		{
		case OpRegimm::BLTZ:
			return Jit::B_branchOffset<OpRegimm::BLTZ>(ctx, instr);
		case OpRegimm::BLTZAL:
			return Jit::B_branchOffset<OpRegimm::BLTZAL>(ctx, instr);
		case OpRegimm::BGEZ:
			return Jit::B_branchOffset<OpRegimm::BGEZ>(ctx, instr);
		case OpRegimm::BGEZAL:
			return Jit::B_branchOffset<OpRegimm::BGEZAL>(ctx, instr);
		default: ;
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
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMULF>(ctx, instr);
		case OpCop2VecFunct::VMULU:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMULU>(ctx, instr);
		case OpCop2VecFunct::VRNDP:
			break;
		case OpCop2VecFunct::VMULQ:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMULQ>(ctx, instr);
		case OpCop2VecFunct::VMUDL:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMUDL>(ctx, instr);
		case OpCop2VecFunct::VMUDM:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMUDM>(ctx, instr);
		case OpCop2VecFunct::VMUDN:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMUDN>(ctx, instr);
		case OpCop2VecFunct::VMUDH:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMUDH>(ctx, instr);
		case OpCop2VecFunct::VMACF:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMACF>(ctx, instr);
		case OpCop2VecFunct::VMACU:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMACU>(ctx, instr);
		case OpCop2VecFunct::VRNDN:
			break;
		case OpCop2VecFunct::VMACQ:
			break;
		case OpCop2VecFunct::VMADL:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMADL>(ctx, instr);
		case OpCop2VecFunct::VMADM:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMADM>(ctx, instr);
		case OpCop2VecFunct::VMADN:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMADN>(ctx, instr);
		case OpCop2VecFunct::VMADH:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMADH>(ctx, instr);
		case OpCop2VecFunct::VADD:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VADD>(ctx, instr);
		case OpCop2VecFunct::VSUB:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VSUB>(ctx, instr);
		case OpCop2VecFunct::VABS:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VABS>(ctx, instr);
		case OpCop2VecFunct::VADDC:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VADDC>(ctx, instr);
		case OpCop2VecFunct::VSUBC:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VSUBC>(ctx, instr);
		case OpCop2VecFunct::VSAR:
			return Jit::Vector::VSAR(ctx, instr);
		case OpCop2VecFunct::VLT:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VLT>(ctx, instr);
		case OpCop2VecFunct::VEQ:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VEQ>(ctx, instr);
		case OpCop2VecFunct::VNE:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VNE>(ctx, instr);
		case OpCop2VecFunct::VGE:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VGE>(ctx, instr);
		case OpCop2VecFunct::VCL:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VCL>(ctx, instr);
		case OpCop2VecFunct::VCH:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VCH>(ctx, instr);
		case OpCop2VecFunct::VCR:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VCR>(ctx, instr);
		case OpCop2VecFunct::VMRG:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VMRG>(ctx, instr);
		case OpCop2VecFunct::VAND:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VAND>(ctx, instr);
		case OpCop2VecFunct::VNAND:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VNAND>(ctx, instr);
		case OpCop2VecFunct::VOR:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VOR>(ctx, instr);
		case OpCop2VecFunct::VNOR:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VNOR>(ctx, instr);
		case OpCop2VecFunct::VXOR:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VXOR>(ctx, instr);
		case OpCop2VecFunct::VNXOR:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::VNXOR>(ctx, instr);
		case OpCop2VecFunct::VRCP:
			return Jit::Vector::VRCP_template<OpCop2VecFunct::VRCP>(ctx, instr);
		case OpCop2VecFunct::VRCPL:
			return Jit::Vector::VRCP_template<OpCop2VecFunct::VRCPL>(ctx, instr);
		case OpCop2VecFunct::VRCPH:
			return Jit::Vector::VRCP_template<OpCop2VecFunct::VRCPH>(ctx, instr);
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

		// undocumented instructions
		case OpCop2VecFunct::VSUT:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VADDB:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VSUBB:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VACCB:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VSUCB:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VSAD:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VSAC:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VSUM:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::Unknown_0x1E:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::Unknown_0x1F:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::Unknown_0x2E:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::Unknown_0x2F:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VEXTT:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VEXTQ:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VEXTN:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::Unknown_0x3B:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VINST:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VINSQ:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VINSN:
			return Jit::Vector::CP2_arithmetic<OpCop2VecFunct::Undocumented_0xFE>(ctx, instr);
		case OpCop2VecFunct::VNULL:
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
			return Jit::Vector::MC2_template<OpCopSub::MFC>(ctx, instr);
		case OpCopSub::MTC:
			return Jit::Vector::MC2_template<OpCopSub::MTC>(ctx, instr);
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
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LBV>(ctx, instr);
		case OpLwc2Funct::LSV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LSV>(ctx, instr);
		case OpLwc2Funct::LLV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LLV>(ctx, instr);
		case OpLwc2Funct::LDV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LDV>(ctx, instr);
		case OpLwc2Funct::LQV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LQV>(ctx, instr);
		case OpLwc2Funct::LRV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LRV>(ctx, instr);
		case OpLwc2Funct::LPV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LPV>(ctx, instr);
		case OpLwc2Funct::LUV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LUV>(ctx, instr);
		case OpLwc2Funct::LHV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LHV>(ctx, instr);
		case OpLwc2Funct::LFV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LFV>(ctx, instr);
		case OpLwc2Funct::LTV:
			return Jit::Vector::LWC2_funct<OpLwc2Funct::LTV>(ctx, instr);
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
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SBV>(ctx, instr);
		case OpSwc2Funct::SSV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SSV>(ctx, instr);
		case OpSwc2Funct::SLV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SLV>(ctx, instr);
		case OpSwc2Funct::SDV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SDV>(ctx, instr);
		case OpSwc2Funct::SQV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SQV>(ctx, instr);
		case OpSwc2Funct::SRV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SRV>(ctx, instr);
		case OpSwc2Funct::SPV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SPV>(ctx, instr);
		case OpSwc2Funct::SUV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SUV>(ctx, instr);
		case OpSwc2Funct::SHV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SHV>(ctx, instr);
		case OpSwc2Funct::SFV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SFV>(ctx, instr);
		case OpSwc2Funct::SWV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::SWV>(ctx, instr);
		case OpSwc2Funct::STV:
			return Jit::Vector::SWC2_funct<OpSwc2Funct::STV>(ctx, instr);
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}
};
