#include "stdafx.h"
#include "Rsp.h"

#include "Rsp_Interface.h"
#include "N64/N64Logger.h"

namespace N64::Rsp_detail
{
	Rsp::Rsp()
	{
		m_status.Halt().Set(true);
	}

	RspCycles Rsp::Step(N64System& n64)
	{
		N64_TRACE(U"rsp step starts pc=0x{:04X}"_fmt(m_pc.Curr()));

		const auto pc = ImemAddr16(m_pc.Curr());
		const auto code = m_imemCache.HitBlockOrRecompile(n64, *this, pc);

		const RspCycles taken = code();
		N64_TRACE(U"rsp step finished pc=0x{:04X} cycles={}"_fmt(m_pc.Curr()), taken);
		return taken;
	}

	uint32 Rsp::ReadPAddr32(PAddr32 paddr)
	{
		return Interface::ReadPAddr32(*this, paddr);
	}

	void Rsp::WritePAddr32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		Interface::WritePAddr32(n64, *this, paddr, value);
	}
}
