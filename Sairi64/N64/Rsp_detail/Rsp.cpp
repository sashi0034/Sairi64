#include "stdafx.h"
#include "Rsp.h"

#include "Rsp_Interface.h"
#include "N64/N64Logger.h"

namespace N64::Rsp_detail
{
	template <int n>
	inline void bitClearSet(Utils::BitAccessor<n, n, uint32> bit, bool clear, bool set)
	{
		if (clear && !set) bit.Set(false);
		if (set && !clear) bit.Set(true);
	}
}

namespace N64::Rsp_detail
{
	Rsp::Rsp()
	{
		m_status.Halt().Set(true);
	}

	RspCycles Rsp::Step(N64System& n64)
	{
		const auto pc = ImemAddr16(m_pc.Curr());
		const auto code = m_imemCache.HitBlockOrRecompile(n64, *this, pc);
		return code();
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
