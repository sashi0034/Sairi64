#include "stdafx.h"
#include "Interrupt.h"

#include "N64System.h"

namespace N64
{
	template <Interruption interrupt> void InterruptRaise(N64System& n64)
	{
		n64.GetMI().ChangeInterrupt<interrupt, true>();
		UpdateInterrupt(n64);
	}

	template <Interruption interrupt>
	void InterruptLower(N64System& n64)
	{
		n64.GetMI().ChangeInterrupt<interrupt, false>();
		UpdateInterrupt(n64);
	}

	void UpdateInterrupt(N64System& n64)
	{
		const bool interrupt =
			static_cast<uint32>(n64.GetMI().GetInterrupt()) &
			static_cast<uint32>(n64.GetMI().GetInterruptMask());
		n64.GetCpu().GetCop0().Reg().cause.Ip2().Set(interrupt);
	}
}
