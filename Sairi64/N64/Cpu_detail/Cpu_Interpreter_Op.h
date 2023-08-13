#pragma once

#include "Cpu_Interpreter.h"
#include "N64/N64Logger.h"

#define END_OP N64_TRACE(instr.Stringify()); return {}

namespace N64::Cpu_detail
{
	struct OperatedUnit
	{
	};
}

class N64::Cpu_detail::Cpu::Interpreter::Op
{
public:
	[[nodiscard]]
	static OperatedUnit ADD(Cpu& cpu, InstructionR instr)
	{
		auto&& gpr = cpu.GetGpr();

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 rt = gpr.Read(instr.Rt());
		const uint32 result = rs + rt;

		gpr.Write(instr.Rd(), (int64)static_cast<int32>(result));

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit SLL(Cpu& cpu, InstructionR instr) // possibly NOP
	{
		const sint32 result = cpu.GetGpr().Read(instr.Rt()) << instr.Sa();
		cpu.GetGpr().Write(instr.Rd(), result);

		END_OP;
	}

	[[nodiscard]]
	static OperatedUnit LUI(Cpu& cpu, InstructionI instr)
	{
		sint64 imm = static_cast<sint16>(instr.Imm());
		imm *= 0x10000; // 負数の左シフトは未定義動作なので乗算で実装
		cpu.GetGpr().Write(instr.Rt(), imm);

		END_OP;
	}

	// TODO: templateでMTC1も表現?
	[[nodiscard]]
	static OperatedUnit MTC0(Cpu& cpu, InstructionCopSub instr)
	{
		const uint32 rt = cpu.GetGpr().Read(instr.Rt());
		cpu.GetCop0().Write32(instr.Rd(), rt);
		END_OP;
	}
};
