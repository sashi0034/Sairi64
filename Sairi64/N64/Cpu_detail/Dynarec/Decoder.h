#pragma once

#include "Jit_Cop.h"
#include "../Cpu_Interpreter_Op_Cop.h"

namespace N64::Cpu_detail::Dynarec
{
	class Decoder;
	using interpret = Cpu::Interpreter::Op;
}

class N64::Cpu_detail::Dynarec::Decoder
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
			return Jit::I_immediateArithmetic<Opcode::ADDIU>(ctx, static_cast<InstructionI>(instr)); // TODO: 例外
		case Opcode::ADDIU:
			return Jit::I_immediateArithmetic<Opcode::ADDIU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SLTI:
			return Jit::SLTI_template<Opcode::SLTI>(ctx, static_cast<InstructionI>(instr));
		case Opcode::SLTIU:
			return Jit::SLTI_template<Opcode::SLTIU>(ctx, static_cast<InstructionI>(instr));
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
			return assembleCP1(ctx, state, static_cast<InstructionCop>(instr));
		case Opcode::CP2:
			break;
		case Opcode::CP3:
			break;
		case Opcode::BEQL:
			return Jit::B_branchOffset<Opcode::BEQL>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BNEL:
			return Jit::B_branchOffset<Opcode::BNEL>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BLEZL:
			return Jit::B_branchOffset<Opcode::BLEZL>(ctx, static_cast<InstructionI>(instr));
		case Opcode::BGTZL:
			return Jit::B_branchOffset<Opcode::BGTZL>(ctx, static_cast<InstructionI>(instr));
		case Opcode::DADDI:
			return Jit::I_immediateArithmetic<Opcode::DADDIU>(ctx, static_cast<InstructionI>(instr)); // TODO: 例外
		case Opcode::DADDIU:
			return Jit::I_immediateArithmetic<Opcode::DADDIU>(ctx, static_cast<InstructionI>(instr));
		case Opcode::LDL:
			return UseInterpreter(ctx, instr, &interpret::LDL);
		case Opcode::LDR:
			return UseInterpreter(ctx, instr, &interpret::LDR);
		case Opcode::RDHWR:
			break;
		case Opcode::LB:
			return Jit::L_load<Opcode::LB>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::LH:
			return Jit::L_load<Opcode::LH>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::LWL:
			break;
		case Opcode::LW:
			return Jit::L_load<Opcode::LW>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::LBU:
			return Jit::L_load<Opcode::LBU>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::LHU:
			return Jit::L_load<Opcode::LHU>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::LWR:
			break;
		case Opcode::LWU:
			return Jit::L_load<Opcode::LWU>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::SB:
			return Jit::S_store<Opcode::SB>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::SH:
			return Jit::S_store<Opcode::SH>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::SWL:
			break;
		case Opcode::SW:
			return Jit::S_store<Opcode::SW>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::SDL:
			return UseInterpreter(ctx, instr, &interpret::SDL);
		case Opcode::SDR:
			return UseInterpreter(ctx, instr, &interpret::SDR);
		case Opcode::SWR:
			break;
		case Opcode::CACHE:
			return Jit::CACHE(static_cast<InstructionR>(instr));
		case Opcode::LL:
			break;
		case Opcode::LWC1:
			break;
		case Opcode::LLD:
			break;
		case Opcode::LDC1:
			break;
		case Opcode::LD:
			return Jit::L_load<Opcode::LD>(ctx, state, static_cast<InstructionI>(instr));
		case Opcode::SC:
			break;
		case Opcode::SWC1:
			break;
		case Opcode::SCD:
			break;
		case Opcode::SDC1:
			break;
		case Opcode::SD:
			return Jit::S_store<Opcode::SD>(ctx, state, static_cast<InstructionI>(instr));
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
			return UseInterpreter(ctx, instr, &interpret::SLLV);
		case OpSpecialFunct::SRLV:
			return UseInterpreter(ctx, instr, &interpret::SRLV);
		case OpSpecialFunct::SRAV:
			return UseInterpreter(ctx, instr, &interpret::SRAV);
		case OpSpecialFunct::JR:
			return Jit::JR_template<OpSpecialFunct::JR>(ctx, instr);
		case OpSpecialFunct::JALR:
			return Jit::JR_template<OpSpecialFunct::JALR>(ctx, instr);
		case OpSpecialFunct::SYSCALL:
			break;
		case OpSpecialFunct::SYNC:
			break;
		case OpSpecialFunct::MFHI:
			return Jit::MF_template<OpSpecialFunct::MFHI>(ctx, instr);
		case OpSpecialFunct::MTHI:
			return Jit::MT_template<OpSpecialFunct::MTHI>(ctx, instr);
		case OpSpecialFunct::MFLO:
			return Jit::MF_template<OpSpecialFunct::MFLO>(ctx, instr);
		case OpSpecialFunct::MTLO:
			return Jit::MT_template<OpSpecialFunct::MTLO>(ctx, instr);
		case OpSpecialFunct::DSLLV:
			break;
		case OpSpecialFunct::DSRLV:
			break;
		case OpSpecialFunct::DSRAV:
			break;
		case OpSpecialFunct::MULT:
			return Jit::MULT_template<OpSpecialFunct::MULT>(ctx, instr);
		case OpSpecialFunct::MULTU:
			return Jit::MULT_template<OpSpecialFunct::MULTU>(ctx, instr);
		case OpSpecialFunct::DIV:
			return UseInterpreter(ctx, instr, &interpret::DIV);
		case OpSpecialFunct::DIVU:
			return UseInterpreter(ctx, instr, &interpret::DIVU);
		case OpSpecialFunct::DMULT:
			break;
		case OpSpecialFunct::DMULTU:
			break;
		case OpSpecialFunct::DDIV:
			break;
		case OpSpecialFunct::DDIVU:
			break;
		case OpSpecialFunct::ADD:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::ADDU>(ctx, instr); // TODO: 例外
		case OpSpecialFunct::ADDU:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::ADDU>(ctx, instr);
		case OpSpecialFunct::SUB:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::SUBU>(ctx, instr); // TODO: 例外
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
			return Jit::SLT_template<OpSpecialFunct::SLT>(ctx, instr);
		case OpSpecialFunct::SLTU:
			return Jit::SLT_template<OpSpecialFunct::SLTU>(ctx, instr);
		case OpSpecialFunct::DADD:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::DADDU>(ctx, instr); // TODO: 例外
		case OpSpecialFunct::DADDU:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::DADDU>(ctx, instr);
		case OpSpecialFunct::DSUB:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::DSUBU>(ctx, instr); // TODO: 例外
		case OpSpecialFunct::DSUBU:
			return Jit::SPECIAL_arithmetic<OpSpecialFunct::DSUBU>(ctx, instr);
		case OpSpecialFunct::TGE:
			break;
		case OpSpecialFunct::TGEU:
			break;
		case OpSpecialFunct::TLT:
			break;
		case OpSpecialFunct::TLTU:
			break;
		case OpSpecialFunct::TEQ:
			return UseInterpreter(ctx, instr, &interpret::TEQ);
		case OpSpecialFunct::TNE:
			return UseInterpreter(ctx, instr, &interpret::TNE);
		case OpSpecialFunct::DSLL:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSLL>(ctx, instr);
		case OpSpecialFunct::DSRL:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSRL>(ctx, instr);
		case OpSpecialFunct::DSRA:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSRA>(ctx, instr);
		case OpSpecialFunct::DSLL32:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSLL32>(ctx, instr);
		case OpSpecialFunct::DSRL32:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSRL32>(ctx, instr);
		case OpSpecialFunct::DSRA32:
			return Jit::SPECIAL_shift<OpSpecialFunct::DSRA32>(ctx, instr);
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleREGIMM(const AssembleContext& ctx, const AssembleState& state, InstructionRegimm instr)
	{
		switch (instr.Sub())
		{
		case OpRegimm::BLTZ:
			return Jit::B_branchOffset<OpRegimm::BLTZ>(ctx, instr);
		case OpRegimm::BGEZ:
			return Jit::B_branchOffset<OpRegimm::BGEZ>(ctx, instr);
		case OpRegimm::BLTZL:
			return Jit::B_branchOffset<OpRegimm::BLTZL>(ctx, instr);
		case OpRegimm::BGEZL:
			return Jit::B_branchOffset<OpRegimm::BGEZL>(ctx, instr);
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
			return Jit::B_branchOffset<OpRegimm::BLTZAL>(ctx, instr);
		case OpRegimm::BGEZAL:
			return Jit::B_branchOffset<OpRegimm::BGEZAL>(ctx, instr);
		case OpRegimm::BGEZALL:
			return Jit::B_branchOffset<OpRegimm::BGEZALL>(ctx, instr);
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleCP0(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MFC:
			return Jit::Cop::MFC0_template<OpCopSub::MFC>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCopSub::DMFC:
			return Jit::Cop::MFC0_template<OpCopSub::DMFC>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCopSub::MTC:
			return Jit::Cop::MTC0_template<OpCopSub::MTC>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCopSub::DMTC:
			return Jit::Cop::MTC0_template<OpCopSub::DMTC>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCopSub::CFC:
			break;
		case OpCopSub::CTC:
			break;
		// @formatter:off
		case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
		case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
		case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
		case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F: // @formatter:on
			return assembleCO0(ctx, state, static_cast<InstructionCop0Co>(instr));
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	[[nodiscard]]
	static DecodedToken assembleCO0(const AssembleContext& ctx, const AssembleState& state, InstructionCop0Co instr)
	{
		switch (instr.Funct())
		{
		case OpCop0CoFunct::TLBR:
			break;
		case OpCop0CoFunct::TLBWI:
			return Jit::Cop::TLBW_template<OpCop0CoFunct::TLBWI>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCop0CoFunct::TLBWR:
			return Jit::Cop::TLBW_template<OpCop0CoFunct::TLBWR>(ctx, static_cast<InstructionCopSub>(instr));
		case OpCop0CoFunct::TLBP:
			break;
		case OpCop0CoFunct::ERET:
			return UseInterpreter(ctx, instr, &interpret::Cop::ERET);
		default: break;
		}

		return AssumeNotImplemented(ctx, instr);
	}

	static DecodedToken assembleCP1(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
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
			return UseInterpreter(ctx, sub, &interpret::Cop::CFC1);
		case OpCopSub::CTC:
			return UseInterpreter(ctx, sub, &interpret::Cop::CTC1);
		// @formatter:off
		case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
		case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
		case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
		case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F: // @formatter:on
			// TODO
			break;
		default: ;
		}

		return AssumeNotImplemented(ctx, instr);
	}
};
