#pragma once
#include "Cpu.h"
#include "Instruction.h"

namespace N64::Cpu_detail
{
	class Cpu::Operation
	{
	public:
		static void OperateInstruction(N64System& n64, Cpu& cpu, Instruction instr);

	private:
		class Impl;
		class Op;
	};
}
