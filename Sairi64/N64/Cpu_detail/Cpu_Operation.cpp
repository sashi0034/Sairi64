#include "stdafx.h"
#include "Cpu_Operation.h"

#include "N64/N64Logger.h"

// @formatter:off
struct OperatedUnit{}; // @formatter:on

using namespace N64::Cpu_detail;

class Cpu::Operation::Impl
{
public:
	[[nodiscard]]
	static OperatedUnit OperateInstructionInternal(N64System& n64, Cpu& cpu, Instruction instr);

private:
	class Op;

	[[nodiscard]]
	static OperatedUnit operateSpecial(N64System& n64, Cpu& cpu, InstructionR instr);
};

#define END_OP N64_TRACE(instr.Stringify()); return {}

class Cpu::Operation::Impl::Op
{
public:
	[[nodiscard]]
	static OperatedUnit ADD(Cpu& cpu, InstructionR instr)
	{
		auto&& gpr = cpu.m_gpr;

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 rt = gpr.Read(instr.Rt());
		const uint32 result = rs + rt;

		gpr.Write(instr.Rd(), (int64)static_cast<int32>(result));

		END_OP;
	}
};

OperatedUnit Cpu::Operation::Impl::OperateInstructionInternal(N64System& n64, Cpu& cpu, Instruction instr)
{
	switch (instr.Op())
	{
	case Opcode::SPECIAL:
		return operateSpecial(n64, cpu, static_cast<InstructionR>(instr));
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
		break;
	case Opcode::CP0:
		break;
	case Opcode::CP1:
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
	case Opcode::LB:
		break;
	case Opcode::LH:
		break;
	case Opcode::LW:
		break;
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
		break;
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

	N64Logger::Abort();
	return {};
}

OperatedUnit Cpu::Operation::Impl::operateSpecial(N64System& n64, Cpu& cpu, InstructionR instr)
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
		break;
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
		break;
	case OpSpecialFunct::SLTU:
		break;
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

	N64Logger::Abort();
	return {};
}

void Cpu::Operation::OperateInstruction(N64System& n64, Cpu& cpu, Instruction instr)
{
	return (void)Impl::OperateInstructionInternal(n64, cpu, instr);
}
