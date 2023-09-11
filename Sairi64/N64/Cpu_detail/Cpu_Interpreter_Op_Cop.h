#pragma once

#include "Cpu_Interpreter_Op.h"

// https://hack64.net/docs/VR43XX.pdf
class N64::Cpu_detail::Cpu::Interpreter::Op::Cop
{
public:
	[[nodiscard]]
	static OperatedUnit MFC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const sint32 value = cpu.GetCop0().Read32(instr.Rd());
		cpu.GetGpr().Write(instr.Rt(), (sint64)value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DMFC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint64 value = cpu.GetCop0().Read64(instr.Rd());
		cpu.GetGpr().Write(instr.Rt(), value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit MTC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint32 rt = cpu.GetGpr().Read(instr.Rt());
		cpu.GetCop0().Write32(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit DMTC0(Cpu& cpu, InstructionCopSub instr)
	{
		BEGIN_OP;
		const uint64 rt = cpu.GetGpr().Read(instr.Rt());
		cpu.GetCop0().Write64(instr.Rd(), rt);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit ERET(Cpu& cpu, InstructionCop0Tlb instr)
	{
		BEGIN_OP;
		auto&& cop0Reg = cpu.GetCop0().Reg();
		if (cop0Reg.status.Erl())
		{
			cpu.GetPc().Change64(cop0Reg.errorEpc);
			cop0Reg.status.Erl().Set(false);
		}
		else
		{
			cpu.GetPc().Change64(cop0Reg.epc);
			cop0Reg.status.Exl().Set(false);
		}
		cpu.GetCop0().SetLLBit(false);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit CFC1(Cpu& cpu, InstructionCop1Sub instr)
	{
		BEGIN_OP;
		auto&& cop1 = cpu.GetCop1();
		sint32 value;
		switch (uint8 fs = instr.Fs())
		{
		case 0:
			value = cop1.Fcr().fcr0;
			break;
		case 31:
			value = cop1.Fcr().fcr31;
			break;
		default:
			N64Logger::Abort(U"undefined cfc1 fs={}"_fmt(fs));
			value = 0;
			break;
		}
		cpu.GetGpr().Write(instr.Rt(), (sint64)value);
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit CTC1(Cpu& cpu, InstructionCop1Sub instr)
	{
		BEGIN_OP;
		uint32 rt = instr.Rt();
		switch (uint8 fs = instr.Fs())
		{
		case 0:
			break; // fcr0 is read-only
		case 31:
			rt &= 0X183FFFF; // mask out bits held 0
			cpu.GetCop1().Fcr().fcr31 = {rt};
			break;
		default:
			N64Logger::Abort(U"undefined ctc1 fs={}"_fmt(fs));
			break;
		}
		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit TLBWI(Cpu& cpu, InstructionCop0Tlb instr)
	{
		BEGIN_OP;
		cpu.GetCop0().GetTlb().WriteEntry(cpu.GetCop0().Reg().index.I());
		END_OP;
	}
};
