#include "stdafx.h"
#include "Cpu_Interpreter.h"

#include "Cpu_Interpreter_Op.h"
#include "N64/N64Logger.h"

using namespace N64::Cpu_detail;

class Cpu::Interpreter::Impl
{
public:
	[[nodiscard]]
	static OperatedUnit InterpretInstructionInternal(N64System& n64, Cpu& cpu, Instruction instr)
	{
		switch (instr.Op())
		{
		case Opcode::SPECIAL:
			return interpretSPECIAL(n64, cpu, static_cast<InstructionR>(instr));
		case Opcode::REGIMM:
			break;
		case Opcode::J:
			break;
		case Opcode::JAL:
			break;
		case Opcode::BEQ:
			break;
		case Opcode::BNE:
			return Op::BNE(cpu, static_cast<InstructionI>(instr));
		case Opcode::BLEZ:
			break;
		case Opcode::BGTZ:
			break;
		case Opcode::ADDI:
			return Op::ADDI(cpu, static_cast<InstructionI>(instr));
		case Opcode::ADDIU:
			return Op::ADDIU(cpu, static_cast<InstructionI>(instr));
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
			return Op::LUI(cpu, static_cast<InstructionI>(instr));
		case Opcode::CP0:
			return interpretCP0(n64, cpu, static_cast<InstructionCop>(instr));
		case Opcode::CP1:
			break;
		case Opcode::BEQL:
			break;
		case Opcode::BNEL:
			return Op::BNEL(cpu, static_cast<InstructionI>(instr));;
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
		case Opcode::LB:
			break;
		case Opcode::LH:
			break;
		case Opcode::LW:
			return Op::LW(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LBU:
			break;
		case Opcode::LHU:
			break;
		case Opcode::LWU:
			break;
		case Opcode::SB:
			break;
		case Opcode::SH:
			break;
		case Opcode::SW:
			break;
		case Opcode::SDL:
			break;
		case Opcode::SDR:
			break;
		case Opcode::CACHE:
			return Op::CACHE(instr);
		case Opcode::LL:
			break;
		case Opcode::LLD:
			break;
		case Opcode::LD:
			break;
		case Opcode::SC:
			break;
		case Opcode::SCD:
			break;
		case Opcode::SD:
			break;
		default: ;
		}

		N64Logger::Abort(U"not implemented: instruction {}"_fmt(instr.OpName()));
		return {};
	}

private:
	[[nodiscard]]
	static OperatedUnit interpretSPECIAL(N64System& n64, Cpu& cpu, InstructionR instr)
	{
		switch (instr.Funct())
		{
		case OpSpecialFunct::SLL:
			return Op::SLL(cpu, instr);
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
		case OpSpecialFunct::MFHI:
			break;
		case OpSpecialFunct::MFLO:
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
			return Op::ADD(cpu, instr);
		case OpSpecialFunct::ADDU:
			return Op::ADDU(cpu, instr);
		case OpSpecialFunct::SUB:
			break;
		case OpSpecialFunct::SUBU:
			break;
		case OpSpecialFunct::AND:
			break;
		case OpSpecialFunct::OR:
			break;
		case OpSpecialFunct::XOR:
			break;
		case OpSpecialFunct::NOR:
			break;
		case OpSpecialFunct::SLT:
			return Op::SLT(cpu, instr);
		case OpSpecialFunct::SLTU:
			return Op::SLTU(cpu, instr);;
		case OpSpecialFunct::DADD:
			break;
		case OpSpecialFunct::DADDU:
			break;
		case OpSpecialFunct::DSUB:
			break;
		case OpSpecialFunct::DSUBU:
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

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	[[nodiscard]]
	static OperatedUnit interpretCP0(N64System& n64, Cpu& cpu, InstructionCop instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MFC:
			break;
		case OpCopSub::DMFC:
			break;
		case OpCopSub::MTC:
			return Op::MTC0(cpu, static_cast<InstructionCopSub>(instr));
		case OpCopSub::DMTC:
			break;
		case OpCopSub::CFC:
			break;
		case OpCopSub::CTC:
			break;
		// @formatter:off
		case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
		case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
		case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
		case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F: // @formatter:on
			// TODO
			break;
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}
};

void Cpu::Interpreter::InterpretInstruction(N64System& n64, Cpu& cpu, Instruction instr)
{
	return (void)Impl::InterpretInstructionInternal(n64, cpu, instr);
}
