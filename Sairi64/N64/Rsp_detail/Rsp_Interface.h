#pragma once
#include "N64/Rsp.h"

namespace N64::Rsp_detail
{
	class N64::Rsp::Interface
	{
	public:
		static uint32 ReadPAddr32(Rsp& rsp, PAddr32 paddr);
		static void WritePAddr32(N64System& n64, Rsp& rsp, PAddr32 paddr, uint32 value);

	private:
		class Impl;
	};
}
