#pragma once
#include "N64/Instruction.h"

namespace N64::Debugger
{
	class CpuDisassembler
	{
	public:
		static String Disassemble(Instruction instr)
		{
			switch (instr.Op())
			{
			// TODO
			default: ;
			}

			return instr.Stringify();
		}

	private:
	};
}
