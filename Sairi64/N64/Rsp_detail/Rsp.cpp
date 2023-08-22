#include "stdafx.h"
#include "Rsp.h"

#include "N64/N64Logger.h"

namespace N64::Rsp_detail
{
	Rsp::Rsp()
	{
		m_status.Halt().Set(true);
	}

	void Rsp::Step(N64System& n64)
	{
		if (m_status.Halt()) return;

		// TODO
	}

	uint32 Rsp::ReadPAddr32(PAddr32 paddr)
	{
		switch (paddr)
		{
		case RspAddress::DmaSpAddr_0x04040000:
			break;
		case RspAddress::DmaRamAddr_0x04040004:
			break;
		case RspAddress::DmaRdLen_0x04040008:
			break;
		case RspAddress::DmaWrLen_0x0404000C:
			break;
		case RspAddress::DmaWrStatus_0x04040010:
			return m_status;
		case RspAddress::DmaFull_0x04040014:
			break;
		case RspAddress::DmaBusy_0x04040018:
			break;
		case RspAddress::Semaphore_0x0404001C:
			break;
		case RspAddress::Pc_0x04080000:
			return m_pc.Curr();
		default: break;
		}

		N64Logger::Abort(U"unsupported rsp read: {:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void Rsp::WritePAddr32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case RspAddress::DmaSpAddr_0x04040000:
			break;
		case RspAddress::DmaRamAddr_0x04040004:
			break;
		case RspAddress::DmaRdLen_0x04040008:
			break;
		case RspAddress::DmaWrLen_0x0404000C:
			break;
		case RspAddress::DmaWrStatus_0x04040010:
			writeStatus(n64, {value});
			return;
		case RspAddress::DmaFull_0x04040014:
			break;
		case RspAddress::DmaBusy_0x04040018:
			break;
		case RspAddress::Semaphore_0x0404001C:
			break;
		case RspAddress::Pc_0x04080000:
			m_pc.Reset(value);
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported rsp write: {:08X}"_fmt(static_cast<uint32>(paddr)));
	}

	template <int n>
	inline void bitClearSet(Utils::BitAccessor<n, n, uint32> bit, bool clear, bool set)
	{
		if (clear && !set) bit.Set(false);
		if (set && !clear) bit.Set(true);
	}

	void Rsp::writeStatus(N64System& n64, SpStatusWrite32 write)
	{
		m_status.Halt().Set(write.ClearHalt()
			                    ? false
			                    : (write.SetHalt() ? true : m_status.Halt()));
		if (write.ClearBroke()) m_status.Broke().Set(false);

		const bool clearInterrupt = write.ClearInterrupt();
		const bool setInterrupt = write.SetInterrupt();
		if (clearInterrupt && setInterrupt == false)
		{
			// TODO: MIと割り込み
		}
		if (clearInterrupt == false && setInterrupt)
		{
			// TODO: MIと割り込み
		}

		bitClearSet(m_status.SingleStep(), write.ClearSStep(), write.SetSStep());
		bitClearSet(m_status.InterruptOnBreak(), write.ClearInterruptOnBreak(), write.SetInterruptOnBreak());
		bitClearSet(m_status.Signal0(), write.ClearSignal0(), write.SetSignal0());
		bitClearSet(m_status.Signal1(), write.ClearSignal1(), write.SetSignal1());
		bitClearSet(m_status.Signal2(), write.ClearSignal2(), write.SetSignal2());
		bitClearSet(m_status.Signal3(), write.ClearSignal3(), write.SetSignal3());
		bitClearSet(m_status.Signal4(), write.ClearSignal4(), write.SetSignal4());
		bitClearSet(m_status.Signal5(), write.ClearSignal5(), write.SetSignal5());
		bitClearSet(m_status.Signal6(), write.ClearSignal6(), write.SetSignal6());
		bitClearSet(m_status.Signal7(), write.ClearSignal7(), write.SetSignal7());
	}
}
