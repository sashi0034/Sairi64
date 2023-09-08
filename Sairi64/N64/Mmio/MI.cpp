#include "stdafx.h"
#include "MI.h"

#include "N64/N64Logger.h"

namespace N64::Mmio
{
	constexpr uint32 miVersion_0x02020102 = 0x02020102;

	uint32 MI::Read32(PAddr32 paddr) const
	{
		switch (paddr)
		{
		case MiAddress::Mode_0x04300000:
			return m_mode;
		case MiAddress::Version_0x04300004:
			return miVersion_0x02020102;
		case MiAddress::Interrupt_0x04300008:
			return m_interrupt;
		case MiAddress::InterruptMask_0x0430000C:
			return m_interruptMask;
		default: break;
		}

		N64Logger::Abort(U"unsupported mi read: {:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void MI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case MiAddress::Mode_0x04300000:
			writeMode(n64, {value});
			return;
		case MiAddress::Version_0x04300004:
			break;
		case MiAddress::Interrupt_0x04300008:
			break;
		case MiAddress::InterruptMask_0x0430000C:
			writeInterruptMask(n64, value);
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported mi write: {:08X}"_fmt(static_cast<uint32>(paddr)));
	}

	void MI::writeMode(N64System& n64, MiModeWrite32 write)
	{
		m_mode.InitLength().Set(write.InitLength());

		if (write.ClearInitMode()) m_mode.InitMode().Set(false);
		if (write.SetInitMode()) m_mode.InitMode().Set(true);

		if (write.ClearEbusTestMode()) m_mode.EbusTestMode().Set(false);
		if (write.SetEbusTestMode()) m_mode.EbusTestMode().Set(true);

		if (write.ClearDpInterrupt())
		{
			InterruptLower<Interruption::DP>(n64);
		}

		if (write.ClearRdramRegisterMode()) m_mode.RdramRegisterMode().Set(false);
		if (write.SetRdramRegisterMode()) m_mode.RdramRegisterMode().Set(true);
	}

	void MI::writeInterruptMask(N64System& n64, uint32 write)
	{
		if (write & (1 << 0)) m_interruptMask.Sp().Set(false);
		if (write & (1 << 1)) m_interruptMask.Sp().Set(true);
		if (write & (1 << 2)) m_interruptMask.Si().Set(false);
		if (write & (1 << 3)) m_interruptMask.Si().Set(true);
		if (write & (1 << 4)) m_interruptMask.Ai().Set(false);
		if (write & (1 << 5)) m_interruptMask.Ai().Set(true);
		if (write & (1 << 6)) m_interruptMask.Vi().Set(false);
		if (write & (1 << 7)) m_interruptMask.Vi().Set(true);
		if (write & (1 << 8)) m_interruptMask.Pi().Set(false);
		if (write & (1 << 9)) m_interruptMask.Pi().Set(true);
		if (write & (1 << 10)) m_interruptMask.Dp().Set(false);
		if (write & (1 << 11)) m_interruptMask.Dp().Set(true);

		UpdateInterrupt(n64);
	}
}
