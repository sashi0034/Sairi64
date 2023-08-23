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
			return interpretREGIMM(n64, cpu, static_cast<InstructionRegimm>(instr));
		case Opcode::J:
			return Op::J(cpu, static_cast<InstructionJ>(instr));
		case Opcode::JAL:
			return Op::JAL(cpu, static_cast<InstructionJ>(instr));
		case Opcode::BEQ:
			return Op::BEQ(cpu, static_cast<InstructionI>(instr));
		case Opcode::BNE:
			return Op::BNE(cpu, static_cast<InstructionI>(instr));
		case Opcode::BLEZ:
			return Op::BLEZ(cpu, static_cast<InstructionI>(instr));
		case Opcode::BGTZ:
			return Op::BGTZ(cpu, static_cast<InstructionI>(instr));
		case Opcode::ADDI:
			return Op::ADDI(cpu, static_cast<InstructionI>(instr));
		case Opcode::ADDIU:
			return Op::ADDIU(cpu, static_cast<InstructionI>(instr));
		case Opcode::SLTI:
			return Op::SLTI(cpu, static_cast<InstructionI>(instr));
		case Opcode::SLTIU:
			return Op::SLTIU(cpu, static_cast<InstructionI>(instr));
		case Opcode::ANDI:
			return Op::ANDI(cpu, static_cast<InstructionI>(instr));
		case Opcode::ORI:
			return Op::ORI(cpu, static_cast<InstructionI>(instr));
		case Opcode::XORI:
			return Op::XORI(cpu, static_cast<InstructionI>(instr));
		case Opcode::LUI:
			return Op::LUI(cpu, static_cast<InstructionI>(instr));
		case Opcode::CP0:
			return interpretCP0(n64, cpu, static_cast<InstructionCop>(instr));
		case Opcode::CP1:
			return interpretCP1(n64, cpu, static_cast<InstructionCop>(instr));
		case Opcode::CP2:
			break;
		case Opcode::CP3:
			break;
		case Opcode::BEQL:
			return Op::BEQL(cpu, static_cast<InstructionI>(instr));
		case Opcode::BNEL:
			return Op::BNEL(cpu, static_cast<InstructionI>(instr));;
		case Opcode::BLEZL:
			return Op::BLEZL(cpu, static_cast<InstructionI>(instr));
		case Opcode::BGTZL:
			return Op::BGTZL(cpu, static_cast<InstructionI>(instr));
		case Opcode::DADDI:
			return Op::DADDI(cpu, static_cast<InstructionI>(instr));
		case Opcode::DADDIU:
			return Op::DADDIU(cpu, static_cast<InstructionI>(instr));
		case Opcode::LDL:
			return Op::LDL(n64, cpu, static_cast<InstructionFi>(instr));
		case Opcode::LDR:
			return Op::LDR(n64, cpu, static_cast<InstructionFi>(instr));
		case Opcode::RDHWR:
			break;
		case Opcode::LB:
			return Op::LB(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LH:
			return Op::LH(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LWL:
			break;
		case Opcode::LW:
			return Op::LW(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LBU:
			return Op::LBU(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LHU:
			return Op::LHU(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::LWR:
			break;
		case Opcode::LWU:
			return Op::LWU(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::SB:
			return Op::SB(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::SH:
			break;
		case Opcode::SWL:
			break;
		case Opcode::SW:
			return Op::SW(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::SDL:
			return Op::SDL(n64, cpu, static_cast<InstructionFi>(instr));
		case Opcode::SDR:
			return Op::SDR(n64, cpu, static_cast<InstructionFi>(instr));
		case Opcode::SWR:
			break;
		case Opcode::CACHE:
			return Op::CACHE(instr);
		case Opcode::LL:
			break;
		case Opcode::LWC1:
			break;
		case Opcode::LLD:
			break;
		case Opcode::LDC1:
			break;
		case Opcode::LD:
			return Op::LD(n64, cpu, static_cast<InstructionI>(instr));
		case Opcode::SC:
			break;
		case Opcode::SWC1:
			break;
		case Opcode::SCD:
			break;
		case Opcode::SDC1:
			break;
		case Opcode::SD:
			return Op::SD(n64, cpu, static_cast<InstructionI>(instr));
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
			return Op::SRL(cpu, instr);
		case OpSpecialFunct::SRA:
			return Op::SRA(cpu, instr);
		case OpSpecialFunct::SLLV:
			return Op::SLLV(cpu, instr);
		case OpSpecialFunct::SRLV:
			return Op::SRLV(cpu, instr);
		case OpSpecialFunct::SRAV:
			return Op::SRAV(cpu, instr);
		case OpSpecialFunct::JR:
			return Op::JR(cpu, instr);
		case OpSpecialFunct::JALR:
			return Op::JALR(cpu, instr);
		case OpSpecialFunct::SYSCALL:
			break; // return Op::SYSCALL(cpu, instr);
		case OpSpecialFunct::SYNC:
			break; // return Op::SYNC(cpu, instr);
		case OpSpecialFunct::MFHI:
			return Op::MFHI(cpu, instr);
		case OpSpecialFunct::MTHI:
			return Op::MTHI(cpu, instr);
		case OpSpecialFunct::MFLO:
			return Op::MFLO(cpu, instr);
		case OpSpecialFunct::MTLO:
			return Op::MTLO(cpu, instr);
		case OpSpecialFunct::DSLLV:
			break; // return Op::DSLLV(cpu, instr);
		case OpSpecialFunct::DSRLV:
			break; // return Op::DSRLV(cpu, instr);
		case OpSpecialFunct::DSRAV:
			break; // return Op::DSRAV(cpu, instr);
		case OpSpecialFunct::MULT:
			return Op::MULT(cpu, instr);
		case OpSpecialFunct::MULTU:
			return Op::MULTU(cpu, instr);
		case OpSpecialFunct::DIV:
			return Op::DIV(cpu, instr);
		case OpSpecialFunct::DIVU:
			return Op::DIVU(cpu, instr);
		case OpSpecialFunct::DMULT:
			break; // return Op::DMULT(cpu, instr);
		case OpSpecialFunct::DMULTU:
			break; // return Op::DMULTU(cpu, instr);
		case OpSpecialFunct::DDIV:
			break; // return Op::DDIV(cpu, instr);
		case OpSpecialFunct::DDIVU:
			break; // return Op::DDIVU(cpu, instr);
		case OpSpecialFunct::ADD:
			return Op::ADD(cpu, instr);
		case OpSpecialFunct::ADDU:
			return Op::ADDU(cpu, instr);
		case OpSpecialFunct::SUB:
			return Op::SUB(cpu, instr);
		case OpSpecialFunct::SUBU:
			return Op::SUBU(cpu, instr);
		case OpSpecialFunct::AND:
			return Op::AND(cpu, instr);
		case OpSpecialFunct::OR:
			return Op::OR(cpu, instr);
		case OpSpecialFunct::XOR:
			return Op::XOR(cpu, instr);
		case OpSpecialFunct::NOR:
			return Op::NOR(cpu, instr);
		case OpSpecialFunct::SLT:
			return Op::SLT(cpu, instr);
		case OpSpecialFunct::SLTU:
			return Op::SLTU(cpu, instr);
		case OpSpecialFunct::DADD:
			break; // return Op::DADD(cpu, instr);
		case OpSpecialFunct::DADDU:
			break; // return Op::DADDU(cpu, instr);
		case OpSpecialFunct::DSUB:
			break; // return Op::DSUB(cpu, instr);
		case OpSpecialFunct::DSUBU:
			break; // return Op::DSUBU(cpu, instr);
		case OpSpecialFunct::TGE:
			break; // return Op::TGE(cpu, instr);
		case OpSpecialFunct::TGEU:
			break; // return Op::TGEU(cpu, instr);
		case OpSpecialFunct::TLT:
			break; // return Op::TLT(cpu, instr);
		case OpSpecialFunct::TLTU:
			break; // return Op::TLTU(cpu, instr);
		case OpSpecialFunct::TEQ:
			return Op::TEQ(cpu, instr);
		case OpSpecialFunct::TNE:
			return Op::TNE(cpu, instr);
		case OpSpecialFunct::DSLL:
			return Op::DSLL(cpu, instr);
		case OpSpecialFunct::DSRL:
			return Op::DSRL(cpu, instr);
		case OpSpecialFunct::DSRA:
			return Op::DSRA(cpu, instr);
		case OpSpecialFunct::DSLL32:
			return Op::DSLL32(cpu, instr);
		case OpSpecialFunct::DSRL32:
			return Op::DSRL32(cpu, instr);
		case OpSpecialFunct::DSRA32:
			return Op::DSRA32(cpu, instr);
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	[[nodiscard]]
	static OperatedUnit interpretREGIMM(N64System& n64, Cpu& cpu, InstructionRegimm instr)
	{
		switch (instr.Sub())
		{
		case OpRegimm::BLTZ:
			return Op::BLTZ(cpu, instr);
		case OpRegimm::BGEZ:
			return Op::BGEZ(cpu, instr);
		case OpRegimm::BLTZL:
			return Op::BLTZL(cpu, instr);
		case OpRegimm::BGEZL:
			return Op::BGEZL(cpu, instr);
		case OpRegimm::TGEI:
			break;
		case OpRegimm::TGEIU:
			break;
		case OpRegimm::TLTI:
			break;
		case OpRegimm::TLTIU:
			break;
		case OpRegimm::TEQI:
			break;
		case OpRegimm::TNEI:
			break;
		case OpRegimm::BLTZAL:
			break;
		case OpRegimm::BGEZAL:
			return Op::BGEZAL(cpu, instr);
		case OpRegimm::BGEZALL:
			return Op::BGEZALL(cpu, instr);
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
			return Op::MFC0(cpu, static_cast<InstructionCopSub>(instr));
		case OpCopSub::DMFC:
			return Op::DMFC0(cpu, static_cast<InstructionCopSub>(instr));
		case OpCopSub::MTC:
			return Op::MTC0(cpu, static_cast<InstructionCopSub>(instr));
		case OpCopSub::DMTC:
			return Op::DMTC0(cpu, static_cast<InstructionCopSub>(instr));
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

	[[nodiscard]]
	static OperatedUnit interpretCP1(N64System& n64, Cpu& cpu, InstructionCop instr)
	{
		switch (const auto sub = static_cast<InstructionCopSub>(instr); instr.Sub())
		{
		case OpCopSub::MFC:
			break;
		case OpCopSub::DMFC:
			break;
		case OpCopSub::MTC:
			break;
		case OpCopSub::DMTC:
			break;
		case OpCopSub::CFC:
			return Op::CFC1(cpu, static_cast<InstructionCop1Sub>(sub));
		case OpCopSub::CTC:
			return Op::CTC1(cpu, static_cast<InstructionCop1Sub>(sub));
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
