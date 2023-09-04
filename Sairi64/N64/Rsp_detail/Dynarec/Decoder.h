#pragma once

#include "Jit.h"
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
			break;
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
		case Opcode::LWC1:
			break;
		case Opcode::LLD:
			break;
		case Opcode::LDC1:
			break;
		case Opcode::LD:
			break;
		case Opcode::SC:
			break;
		case Opcode::SWC1:
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
};
