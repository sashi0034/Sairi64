#include "stdafx.h"
#include "Cpu.h"

#include "Cpu_Operation.h"
#include "Instruction.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	void Cpu::Step(N64System& n64)
	{
		N64_TRACE(U"CPU cycle starts PC={:#018x}"_fmt(m_pc.Curr()));

		const Optional<PAddr32> paddrOfPc = Mmu::ResolveVAddr(*this, m_pc.Curr());
		if (paddrOfPc.has_value() == false)
		{
			// TODO
			N64Logger::Abort();
		}

		const Instruction fetchedInstr = {Mmu::ReadPaddr32(n64, paddrOfPc.value())};
		N64_TRACE(U"fetched instr={:08X} from pc={:016X}"_fmt(fetchedInstr.Raw(), m_pc.Curr()));

		m_pc.Step();

		Operation::OperateInstruction(n64, *this, fetchedInstr);
	}
}
