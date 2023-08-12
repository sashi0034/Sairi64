﻿#pragma once

#include "Cpu_Operation.h"
#include "N64/N64Logger.h"

#define END_OP N64_TRACE(instr.Stringify()); return {}

namespace N64::Cpu_detail
{
	struct OperatedUnit
	{
	};
}

class N64::Cpu_detail::Cpu::Operation::Op
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
		const int32 result = cpu.GetGpr().Read(instr.Rt()) << instr.Sa();
		cpu.GetGpr().Write(instr.Rd(), result);

		END_OP;
	}
};
