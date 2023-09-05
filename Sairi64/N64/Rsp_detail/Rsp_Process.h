#pragma once
#include "Rsp.h"

#if (not defined(RSP_PROCESS_INTERNAL))
#error "This file is an internal file"
#endif

class N64::Rsp_detail::Rsp::Process
{
public:
	static PcRaw& AccessPc(Rsp& rsp)
	{
		return rsp.m_pc.Raw();
	}

	static GprRaw& AccessGpr(Rsp& rsp)
	{
		return rsp.m_gpr.Raw();
	}

	static VU& AccessVU(Rsp& rsp)
	{
		return rsp.m_vu;
	}

	static void SpBreak(N64System& n64, Rsp& rsp)
	{
		rsp.Status().Halt().Set(true);
		rsp.Status().Broke().Set(true);

		InterruptRaise<Interruption::SP>(n64);
	}
};
