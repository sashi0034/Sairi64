#pragma once
#include "N64/Rdp.h"

namespace N64::Rdp_detail
{
	class Rdp::Interface
	{
	public:
		// https://github.com/SimoneN64/Kaizen/blob/75b98053c6a7353f6c6a9de04888ad54d99e0b00/src/backend/core/RDP.cpp#L53
		static void WriteStatus(N64System& n64, Rdp& rdp, DpcStatusWrite32 write)
		{
			auto&& dpc = rdp.m_dpc;
			bitClearSet(dpc.status.XbusDmemDma(), write.ClearXbusDmemDma(), write.SetXbusDmemDma());
			if (write.ClearFreeze())
			{
				dpc.status.Freeze().Set(false);
			}

			if (write.SetFreeze())
			{
				dpc.status.Freeze().Set(true);
			}

			bitClearSet(dpc.status.Flush(), write.ClearFlush(), write.SetFlush());
			bitClearSet(dpc.status.CmdBusy(), write.ClearCmd(), false);
			if (write.ClearClock()) dpc.clock = 0;
			bitClearSet(dpc.status.PipeBusy(), write.ClearPipe(), false);
			bitClearSet(dpc.status.TmemBusy(), write.ClearTmem(), false);

			rdp.checkRunCommand(n64);
		}

		static void WriteStart(Rdp& rdp, uint32 value)
		{
			auto&& dpc = rdp.m_dpc;
			if (dpc.status.StartValid() == false)
			{
				dpc.start = value & 0xFFFFF8;
				dpc.status.StartValid().Set(true);
			}
		}

		static void WriteEnd(N64System& n64, Rdp& rdp, uint32 value)
		{
			auto&& dpc = rdp.m_dpc;
			dpc.end = value & 0xFFFFF8;
			if (dpc.status.StartValid())
			{
				dpc.current = dpc.start;
				dpc.status.StartValid().Set(false);
			}
			rdp.checkRunCommand(n64);
		}

	private:
		template <int n>
		static inline void bitClearSet(BitAccessor<n, n, uint32> bit, bool clear, bool set)
		{
			if (clear && !set) bit.Set(false);
			if (set && !clear) bit.Set(true);
		}
	};
}
