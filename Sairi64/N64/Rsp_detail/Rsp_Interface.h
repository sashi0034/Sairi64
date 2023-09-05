#pragma once
#include "N64/Rsp.h"

namespace N64::Rsp_detail
{
	class N64::Rsp::Interface
	{
	public:
		static uint32 ReadPAddr32(Rsp& rsp, PAddr32 paddr);
		static void WritePAddr32(N64System& n64, Rsp& rsp, PAddr32 paddr, uint32 value);

		N64_ABI static void WriteSpCop0(N64System& n64, Rsp& rsp, uint8 index, uint32 value);
		N64_ABI static uint32 ReadSpCop0(N64System& n64, Rsp& rsp, uint8 index);

	private:
		class Impl;
	};
}
