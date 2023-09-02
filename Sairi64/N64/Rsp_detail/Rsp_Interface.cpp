#include "stdafx.h"
#include "Rsp_Interface.h"

#include "N64/Interrupt.h"
#include "N64/N64Logger.h"

class N64::Rsp_detail::Rsp::Interface::Impl
{
public:
	static bool acquireSemaphore(Rsp& rsp)
	{
		if (rsp.m_semaphore) return true;
		rsp.m_semaphore = true;
		return false; // acquire
	}

	static void writeStatus(N64System& n64, Rsp& rsp, SpStatusWrite32 write)
	{
		rsp.m_status.Halt().Set(write.ClearHalt()
			                        ? false
			                        : (write.SetHalt() ? true : rsp.m_status.Halt()));
		if (write.ClearBroke()) rsp.m_status.Broke().Set(false);

		const bool clearInterrupt = write.ClearInterrupt();
		const bool setInterrupt = write.SetInterrupt();
		if (clearInterrupt && setInterrupt == false)
		{
			InterruptLower<Interruption::SP>(n64);
		}
		if (clearInterrupt == false && setInterrupt)
		{
			InterruptRaise<Interruption::SP>(n64);
		}

		bitClearSet(rsp.m_status.SingleStep(), write.ClearSStep(), write.SetSStep());
		bitClearSet(rsp.m_status.InterruptOnBreak(), write.ClearInterruptOnBreak(), write.SetInterruptOnBreak());
		bitClearSet(rsp.m_status.Signal0(), write.ClearSignal0(), write.SetSignal0());
		bitClearSet(rsp.m_status.Signal1(), write.ClearSignal1(), write.SetSignal1());
		bitClearSet(rsp.m_status.Signal2(), write.ClearSignal2(), write.SetSignal2());
		bitClearSet(rsp.m_status.Signal3(), write.ClearSignal3(), write.SetSignal3());
		bitClearSet(rsp.m_status.Signal4(), write.ClearSignal4(), write.SetSignal4());
		bitClearSet(rsp.m_status.Signal5(), write.ClearSignal5(), write.SetSignal5());
		bitClearSet(rsp.m_status.Signal6(), write.ClearSignal6(), write.SetSignal6());
		bitClearSet(rsp.m_status.Signal7(), write.ClearSignal7(), write.SetSignal7());
	}

private:
	template <int n>
	static inline void bitClearSet(Utils::BitAccessor<n, n, uint32> bit, bool clear, bool set)
	{
		if (clear && !set) bit.Set(false);
		if (set && !clear) bit.Set(true);
	}
};

namespace N64::Rsp_detail
{
	uint32 Rsp::Interface::ReadPAddr32(Rsp& rsp, PAddr32 paddr)
	{
		switch (paddr)
		{
		case RspAddress::DmaSpAddr_0x04040000:
			break;
		case RspAddress::DmaRamAddr_0x04040004:
			break;
		case RspAddress::DmaRdLen_0x04040008: [[fallthrough]];
		case RspAddress::DmaWrLen_0x0404000C:
			return rsp.m_dmaLength;
		case RspAddress::DmaWrStatus_0x04040010:
			return rsp.m_status;
		case RspAddress::DmaFull_0x04040014:
			return rsp.m_status.DmaFull();
		case RspAddress::DmaBusy_0x04040018:
			return 0;
		case RspAddress::Semaphore_0x0404001C:
			return Impl::acquireSemaphore(rsp);
		case RspAddress::Pc_0x04080000:
			return rsp.m_pc.Curr();
		default: break;
		}

		N64Logger::Abort(U"unsupported rsp read: {:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void Rsp::Interface::WritePAddr32(N64System& n64, Rsp& rsp, PAddr32 paddr, uint32 value)
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
			Impl::writeStatus(n64, rsp, {value});
			return;
		case RspAddress::DmaFull_0x04040014:
			break;
		case RspAddress::DmaBusy_0x04040018:
			break;
		case RspAddress::Semaphore_0x0404001C:
			rsp.m_semaphore = false;
			return;
		case RspAddress::Pc_0x04080000:
			rsp.m_pc.Reset(value);
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported rsp write: {:08X}"_fmt(static_cast<uint32>(paddr)));
	}
}
