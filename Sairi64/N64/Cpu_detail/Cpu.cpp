#include "stdafx.h"
#include "Cpu.h"

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

		// TODO: memory

		m_pc.Step();
	}
}
