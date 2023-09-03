#pragma once
#include "Cpu.h"
#include "N64/Instruction.h"

namespace N64::Cpu_detail
{
	class Cpu::Interpreter
	{
	public:
		static void InterpretInstruction(N64System& n64, Cpu& cpu, Instruction instr);
		class Op;

	private:
		class Impl;
	};
}
