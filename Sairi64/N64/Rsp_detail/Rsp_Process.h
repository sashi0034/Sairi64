#pragma once
#include "Rsp.h"

#if (not defined(RSP_PROCESS_INTERNAL))
#error "This file is an internal file"
#endif

namespace N64::Rsp_detail
{
	class Rsp::Process
	{
	public:
		static PcRaw& AccessPc(Rsp& rsp)
		{
			return rsp.m_pc.Raw();
		}
	};
}
