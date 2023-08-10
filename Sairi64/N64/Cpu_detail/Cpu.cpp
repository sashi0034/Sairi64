#include "stdafx.h"
#include "Cpu.h"

#include "N64/N64Logger.h"

namespace N64::Cpu_detail
{
	void Cpu::Step(N64System& n64)
	{
		N64_TRACE(U"CPU cycle starts PC={:#018x}"_fmt(m_pc.Curr()));

		m_pc.Step();
	}
}
