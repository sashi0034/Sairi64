#include "stdafx.h"
#include "DebugDisassembler.h"

namespace N64::Debugger
{
	class DebugDisassembler
	{
	public:
		static String Disassemble(Instruction instr)
		{
			switch (instr.Op())
			{
			case Opcode::SPECIAL:
				return by_<InstructionR>(instr);
			case Opcode::REGIMM:
				return by_<InstructionRegimm>(instr);
			case Opcode::J:
				return by_<InstructionJ>(instr);
			case Opcode::JAL:
				return by_<InstructionJ>(instr);
			case Opcode::BEQ:
				return by_<InstructionI>(instr);
			case Opcode::BNE:
				return by_<InstructionI>(instr);
			case Opcode::BLEZ:
				return by_<InstructionI>(instr);
			case Opcode::BGTZ:
				return by_<InstructionI>(instr);
			case Opcode::ADDI:
				return by_<InstructionI>(instr);
			case Opcode::ADDIU:
				return by_<InstructionI>(instr);
			case Opcode::SLTI:
				return by_<InstructionI>(instr);
			case Opcode::SLTIU:
				return by_<InstructionI>(instr);
			case Opcode::ANDI:
				return by_<InstructionI>(instr);
			case Opcode::ORI:
				return by_<InstructionI>(instr);
			case Opcode::XORI:
				return by_<InstructionI>(instr);
			case Opcode::LUI:
				return by_<InstructionI>(instr);
			case Opcode::CP0:
				return disassembleCP0(static_cast<InstructionCop>(instr));
			case Opcode::CP1:
				return disassembleCP1(static_cast<InstructionCop>(instr));
			case Opcode::CP2:
				return disassembleCP2(static_cast<InstructionCop2Vec>(instr));
			case Opcode::CP3:
				break;
			case Opcode::BEQL:
				return by_<InstructionI>(instr);
			case Opcode::BNEL:
				return by_<InstructionI>(instr);
			case Opcode::BLEZL:
				return by_<InstructionI>(instr);
			case Opcode::BGTZL:
				return by_<InstructionI>(instr);
			case Opcode::DADDI:
				return by_<InstructionI>(instr);
			case Opcode::DADDIU:
				return by_<InstructionI>(instr);
			case Opcode::LDL:
				return by_<InstructionFi>(instr);
			case Opcode::LDR:
				return by_<InstructionFi>(instr);
			case Opcode::RDHWR:
				break;
			case Opcode::LB:
				return by_<InstructionI>(instr);
			case Opcode::LH:
				return by_<InstructionI>(instr);
			case Opcode::LWL:
				break;
			case Opcode::LW:
				return by_<InstructionI>(instr);
			case Opcode::LBU:
				return by_<InstructionI>(instr);
			case Opcode::LHU:
				return by_<InstructionI>(instr);
			case Opcode::LWR:
				break;
			case Opcode::LWU:
				return by_<InstructionI>(instr);
			case Opcode::SB:
				return by_<InstructionI>(instr);
			case Opcode::SH:
				return by_<InstructionI>(instr);
			case Opcode::SWL:
				break;
			case Opcode::SW:
				return by_<InstructionI>(instr);
			case Opcode::SDL:
				return by_<InstructionFi>(instr);
			case Opcode::SDR:
				return by_<InstructionFi>(instr);
			case Opcode::SWR:
				break;
			case Opcode::CACHE:
				break;
			case Opcode::LL:
				break;
			case Opcode::LWC1:
				return by_<InstructionFi>(instr);
			case Opcode::LWC2:
				return by_<InstructionLv>(instr);
			case Opcode::LLD:
				break;
			case Opcode::LDC1:
				return by_<InstructionFi>(instr);
			case Opcode::LD:
				break;
			case Opcode::SC:
				break;
			case Opcode::SWC1:
				return by_<InstructionFi>(instr);
			case Opcode::SWC2:
				return by_<InstructionSv>(instr);
			case Opcode::SCD:
				break;
			case Opcode::SDC1:
				return by_<InstructionFi>(instr);
			case Opcode::SD:
				return by_<InstructionI>(instr);
			case Opcode::Invalid_0xFF:
				break;
			default: ;
			}

			return instr.Stringify();
		}

	private:
		template <typename Casted, typename Instr>
		static String by_(Instr instr)
		{
			static_assert(std::convertible_to<Casted, Instr>);
			return static_cast<Casted>(instr).Stringify();
		}

		template <typename Instr>
		static String as_(Instr instr)
		{
			static_assert(std::convertible_to<Instr, Instr>);
			return static_cast<Instr>(instr).Stringify();
		}

		static String disassembleCP0(InstructionCop instr)
		{
			switch (instr.Sub())
			{
				// @formatter:off
			case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
			case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
			case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
			case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F:
				// @formatter:on
				return by_<InstructionCop0Tlb>(instr);
			default: ;
			}

			return by_<InstructionCopSub>(instr);
		}

		static String disassembleCP1(InstructionCop instr)
		{
			switch (instr.Sub())
			{
			case OpCopSub::BC:
				return by_<InstructionCop1Bc>(instr);
			case OpCopSub::CO_0x10:
			case OpCopSub::CO_0x11:
			case OpCopSub::CO_0x14:
			case OpCopSub::CO_0x15:
				return by_<InstructionCop1Fmt>(instr);
			default: ;
			}

			return by_<InstructionCopSub>(instr);
		}

		static String disassembleCP2(InstructionCop2Vec instr)
		{
			if (instr.IsFunct())
				return by_<InstructionCop2VecFunct>(instr);
			else
				return by_<InstructionCop2VecSub>(instr);
		}
	};

	String DebugDisassemble(Instruction instr)
	{
		return DebugDisassembler::Disassemble(instr);
	}
}
