#include "stdafx.h"
#include "Rsp_Interface.h"

#include "N64/Interrupt.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Rsp_detail
{
	enum class DmaType
	{
		RdramToSp,
		SpToRdram,
	};

	template <DmaType dma>
	static inline void stepDma(
		N64System& n64,
		std::span<uint8> spMem, uint32 spAddr,
		std::span<uint8> rdram, uint32 dramAddr,
		uint32 transferLength, bool isImemTarget)
	{
		// データ転送
		for (int x = 0; x < transferLength; ++x)
		{
			const uint16 targetSpAddr = (spAddr + x) & SpMemoryMask_0xFFF;
			if constexpr (dma == DmaType::RdramToSp)
			{
				spMem[isImemTarget ? targetSpAddr : EndianAddress<uint8>(targetSpAddr)] = rdram[dramAddr + x];
			}
			else if constexpr (dma == DmaType::SpToRdram)
			{
				rdram[dramAddr + x] = spMem[isImemTarget ? targetSpAddr : EndianAddress<uint8>(targetSpAddr)];
			}
			else static_assert(AlwaysFalseValue<DmaType, dma>);
		}

		// キャッシュ無効化
		if constexpr (dma == DmaType::RdramToSp)
		{
			if (isImemTarget)
			{
				n64.GetRsp().ImemCache().InvalidBlockBetween(
					ImemAddr16::Masked_0xFFF(spAddr), ImemAddr16::Masked_0xFFF(spAddr + transferLength - 1));
			}
		}
		else if constexpr (dma == DmaType::SpToRdram)
		{
			// 念のため
			n64.GetCpu().RecompiledCache().CheckInvalidatePageBetween(
				PAddr32(dramAddr), PAddr32(dramAddr + transferLength - 1));
		}
	}
}

class N64::Rsp_detail::Rsp::Interface::Impl
{
public:
	static bool AcquireSemaphore(Rsp& rsp)
	{
		if (rsp.m_semaphore) return true;
		rsp.m_semaphore = true;
		return false; // acquire
	}

	static void ReleaseSemaphore(Rsp& rsp)
	{
		rsp.m_semaphore = false;
	}

	static void WriteStatus(N64System& n64, Rsp& rsp, SpStatusWrite32 write)
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

	// https://github.com/Dillonb/n64/blob/42e5ad9887ce077dd9d9ab97a3a3e03086f7e2d8/src/cpu/rsp.h#L146
	// https://github.com/SimoneN64/Kaizen/blob/56ab73865271635d887eab96a0e51873347abe77/src/backend/core/RSP.hpp#L360
	template <DmaType dma>
	static void Dma(N64System& n64, Rsp& rsp, SpDmaLength32 dmaLength)
	{
		rsp.m_dmaLength = {dmaLength};
		rsp.m_dmaSpAddr = rsp.m_shadowDmaSpAddr;
		rsp.m_dmaDramAddr = rsp.m_shadowDmaDramAddr;

		const bool isImemTarget = rsp.m_dmaSpAddr.Bank();
		if constexpr (dma == DmaType::SpToRdram)
			N64_TRACE(Rsp, U"dma {} -> rdram"_fmt(isImemTarget ? U"imem" : U"dmem"));
		else if constexpr (dma == DmaType::RdramToSp)
			N64_TRACE(Rsp, U"dma rdram -> {}"_fmt(isImemTarget ? U"imem" : U"dmem"));

		const uint32 transferLength = ((dmaLength.Length() + 1) + 0x7) & ~0x7;

		uint32 spAddr = rsp.m_dmaSpAddr & 0xFF8;
		uint32 dramAddr = rsp.m_dmaDramAddr & 0xFFFFF8;

		const std::span<uint8> spMem = isImemTarget ? rsp.Imem() : rsp.Dmem();
		auto&& rdram = n64.GetMemory().Rdram();

		for (int i = 0; i < dmaLength.Count() + 1; ++i)
		{
			// データ転送
			stepDma<dma>(n64, spMem, spAddr, rdram, dramAddr, transferLength, isImemTarget);

			const uint32 skip = i == dmaLength.Count() ? 0 : dmaLength.Skip();
			dramAddr += transferLength + skip;
			dramAddr &= 0xFFFFF8;
			spAddr += transferLength;
			spAddr &= 0xFF8;
		}

		rsp.m_dmaDramAddr.Address().Set(dramAddr);
		rsp.m_dmaSpAddr.Address().Set(spAddr);
		rsp.m_dmaSpAddr.Bank().Set(isImemTarget);
		rsp.m_dmaLength = 0xFF8 | (rsp.m_dmaLength.Skip() << 20);
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
			return rsp.m_dmaSpAddr;
		case RspAddress::DmaDramAddr_0x04040004:
			return rsp.m_dmaDramAddr;
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
			return Impl::AcquireSemaphore(rsp);
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
			rsp.m_shadowDmaSpAddr = {value};
			return;
		case RspAddress::DmaDramAddr_0x04040004:
			rsp.m_shadowDmaDramAddr = {value};
			return;
		case RspAddress::DmaRdLen_0x04040008:
			Impl::Dma<DmaType::RdramToSp>(n64, rsp, value);
			return;
		case RspAddress::DmaWrLen_0x0404000C:
			Impl::Dma<DmaType::SpToRdram>(n64, rsp, value);
			return;
		case RspAddress::DmaWrStatus_0x04040010:
			Impl::WriteStatus(n64, rsp, {value});
			return;
		case RspAddress::DmaFull_0x04040014:
			break;
		case RspAddress::DmaBusy_0x04040018:
			break;
		case RspAddress::Semaphore_0x0404001C:
			Impl::ReleaseSemaphore(rsp);
			return;
		case RspAddress::Pc_0x04080000:
			rsp.m_pc.Reset(value);
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported rsp write: {:08X}"_fmt(static_cast<uint32>(paddr)));
	}

	void Rsp::Interface::WriteSpCop0(N64System& n64, Rsp& rsp, uint8 index, uint32 value)
	{
		switch (index)
		{
		case 0:
			rsp.m_shadowDmaSpAddr = {value};
			return;
		case 1:
			rsp.m_shadowDmaDramAddr = {value};
			return;
		case 2:
			Impl::Dma<DmaType::RdramToSp>(n64, rsp, value);
			return;
		case 3:
			Impl::Dma<DmaType::SpToRdram>(n64, rsp, value);
			return;
		case 4:
			Impl::WriteStatus(n64, rsp, {value});
			return;
		case 7:
			if (value == 0)
				Impl::ReleaseSemaphore(rsp);
			else
				N64Logger::Abort(U"could not release semaphore with non-zero value");
			return;
		case 8:
			n64.GetRdp().WriteStart(value);
			return;
		case 9:
			n64.GetRdp().WriteEnd(n64, value);
			return;
		case 11:
			n64.GetRdp().WriteStatus(n64, {value});
			return;
		default: ;
		}
		N64Logger::Abort(U"unsupported sp-cop0 write: index={}"_fmt(index));
	}

	uint32 Rsp::Interface::ReadSpCop0(N64System& n64, Rsp& rsp, uint8 index)
	{
		switch (index)
		{
		case 0:
			return rsp.m_dmaSpAddr;
		case 1:
			return rsp.m_dmaDramAddr;
		case 2: [[fallthrough]];
		case 3:
			return rsp.m_dmaLength;
		case 4:
			return rsp.m_status;
		case 5:
			return rsp.m_status.DmaFull();
		case 6:
			return rsp.m_status.DmaBusy();
		case 7:
			return Impl::AcquireSemaphore(rsp);
		case 8: // TODO
		case 9: // TODO
		case 10: // TODO
		case 11: // TODO
		case 12: // TODO
		case 13: // TODO
		case 14: // TODO
		case 15: // TODO
		default: ;
		}

		N64Logger::Abort(U"unsupported sp-cop0 read: index={}"_fmt(index));
		return {};
	}
}
