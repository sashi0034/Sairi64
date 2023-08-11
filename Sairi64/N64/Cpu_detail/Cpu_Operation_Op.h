#pragma once

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
		auto&& gpr = cpu.m_gpr;

		const uint32 rs = gpr.Read(instr.Rs());
		const uint32 rt = gpr.Read(instr.Rt());
		const uint32 result = rs + rt;

		gpr.Write(instr.Rd(), (int64)static_cast<int32>(result));

		END_OP;
	}
};
