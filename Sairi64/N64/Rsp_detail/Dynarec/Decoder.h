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
			break;
		case Opcode::ADDIU:
			break;
		case Opcode::SLTI:
			break;
		case Opcode::SLTIU:
			break;
		case Opcode::ANDI:
			break;
		case Opcode::ORI:
			break;
		case Opcode::XORI:
			break;
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
			break;
		case Opcode::LBU:
			break;
		case Opcode::LHU:
			break;
		case Opcode::LWR:
			break;
		case Opcode::LWU:
			break;
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
