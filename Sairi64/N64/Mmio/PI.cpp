#include "stdafx.h"
#include "PI.h"

#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"

namespace N64::Mmio
{
	void writeCartridgeByte(Memory& memory, uint32 address, uint8 value)
	{
		if (Mmu::PMap::CartridgeSram.IsBetween(address))
		{
			memory.Sram()[address - Mmu::PMap::CartridgeSram.base] = value;
			return;
		}
		N64Logger::Abort(U"invalid write cartridge at {:08X}"_fmt(address));
	}

	uint8 readCartridgeByte(Memory& memory, uint32 address)
	{
		if (Mmu::PMap::CartridgeRom.IsBetween(address))
			return memory.GetRom().Data()[EndianAddress<uint8>(address - Mmu::PMap::CartridgeRom.base)];
		if (Mmu::PMap::CartridgeSram.IsBetween(address))
			return memory.Sram()[address - Mmu::PMap::CartridgeSram.base];
		N64Logger::Abort(U"invalid read cartridge at {:08X}"_fmt(address));
		return {};
	}

	enum class PiDma
	{
		RdramToCartridge,
		CartridgeToRdram
	};
}

class N64::Mmio::PI::Impl
{
public:
	template <PiDma dma>
	static void StartDma(N64System& n64, PI& pi, uint32 lengthValue)
	{
		if constexpr (dma == PiDma::RdramToCartridge)
			pi.m_rdLen = lengthValue;
		else if constexpr (dma == PiDma::CartridgeToRdram)
			pi.m_wrLen = lengthValue;
		else static_assert(AlwaysFalseValue<PiDma, dma>);

		const uint32 dramAddr = pi.m_dramAddr & 0x7FFFFE;
		const uint32 cartAddr = pi.m_cartAddr & 0xFFFFFFFE;
		const uint32 transferLength = [&]()
		{
			uint32 value = (lengthValue & 0x00FF'FFFF) + 1;
			if (dramAddr & 0x7) value -= dramAddr & 0x7;
			return value;
		}();

		N64_TRACE(Mmio, dma == PiDma::RdramToCartridge
		          ? U"start pi dma rdram to cartridge: {} bytes {:08X} -> {:08X}"_fmt(
			          transferLength, cartAddr, dramAddr)
		          : U"start pi dma cartridge to rdram: {} bytes {:08X} -> {:08X}"_fmt(
			          transferLength, cartAddr, dramAddr));

		if (cartAddr < Mmu::PMap::N64DdIplRom.base)
			N64Logger::Abort(U"pi dma transfer card address is out of range: {}"_fmt(cartAddr));

		// 転送
		dmaTransfer<dma>(n64, dramAddr, cartAddr, transferLength);

		pi.m_status.DmaBusy().Set(true);
		pi.m_status.IoBusy().Set(true);

		const uint32 transferCycles = transferLength / 8; // ?
		n64.GetScheduler().ScheduleEvent(transferCycles, [&n64, &pi]()
		{
			N64_TRACE(Mmio, U"completed pi dma");

			pi.m_status.DmaBusy().Set(false);
			pi.m_status.IoBusy().Set(false);

			InterruptRaise<Interruption::PI>(n64);
		});
	}

private:
	template <PiDma dma>
	static void dmaTransfer(N64System& n64, uint32 dramAddr, uint32 cartAddr, uint32 transferLength);
};

namespace N64::Mmio
{
	template <> void PI::Impl::dmaTransfer<PiDma::RdramToCartridge>(
		N64System& n64, uint32 dramAddr, uint32 cartAddr, uint32 transferLength)
	{
		for (uint32 i = 0; i < transferLength; ++i)
		{
			const uint8 value = n64.GetMemory().Rdram()[EndianAddress<uint8>(dramAddr + i)];
			writeCartridgeByte(n64.GetMemory(), cartAddr + i, value);
		}
	}

	template <> void PI::Impl::dmaTransfer<PiDma::CartridgeToRdram>(
		N64System& n64, uint32 dramAddr, uint32 cartAddr, uint32 transferLength)
	{
		for (uint32 i = 0; i < transferLength; ++i)
		{
			n64.GetMemory().Rdram()[EndianAddress<uint8>(dramAddr + i)] = readCartridgeByte(
				n64.GetMemory(), cartAddr + i);
		}

		// 対応する再コンパイル済みキャッシュも無効に
		n64.GetCpu().RecompiledCache().CheckInvalidatePageBetween(
			PAddr32(EndianAddress<uint8>(dramAddr)), PAddr32(EndianAddress<uint8>(dramAddr + transferLength - 1)));
	}

	uint32 PI::Read32(N64System& n64, PAddr32 paddr) const
	{
		switch (paddr)
		{
		case PiAddress::DramAddr_0x04600000:
			return m_dramAddr;
		case PiAddress::CartAddr_0x04600004:
			return m_cartAddr;
		case PiAddress::RdLen_0x04600008:
			return m_rdLen;
		case PiAddress::WrLen_0x0460000C:
			return m_wrLen;
		case PiAddress::Status_0x04600010: {
			PiStatus32 status = m_status;
			status.Interrupt().Set(n64.GetMI().GetInterrupt().Pi());
			return status;
		}
		case PiAddress::BsdDom1Lat_0x04600014:
			return m_bsdDom1Lat;
		case PiAddress::BsdDom1Pwd_0x04600018:
			return m_bsdDom1Pwd;
		case PiAddress::BsdDom1Pgs_0x0460001C:
			return m_bsdDom1Pgs;
		case PiAddress::BsdDom1Rls_0x04600020:
			return m_bsdDom1Rls;
		case PiAddress::BsdDom2Lat_0x04600024:
			return m_bsdDom2Lat;
		case PiAddress::BsdDom2Pwd_0x04600028:
			return m_bsdDom2Pwd;
		case PiAddress::BsdDom2Pgs_0x0460002C:
			return m_bsdDom2Pgs;
		case PiAddress::BsdDom2Rls_0x04600030:
			return m_bsdDom2Rls;
		default: break;
		}

		N64Logger::Abort(U"unsupported pi read: paddr={:08X}"_fmt(static_cast<uint32>(paddr)));
		return 0;
	}

	void PI::Write32(N64System& n64, PAddr32 paddr, uint32 value)
	{
		switch (paddr)
		{
		case PiAddress::DramAddr_0x04600000:
			m_dramAddr = value & 0x00FF'FFFF;
			return;
		case PiAddress::CartAddr_0x04600004:
			m_cartAddr = value;
			return;
		case PiAddress::RdLen_0x04600008:
			Impl::StartDma<PiDma::RdramToCartridge>(n64, *this, value);
			return;
		case PiAddress::WrLen_0x0460000C:
			Impl::StartDma<PiDma::CartridgeToRdram>(n64, *this, value);
			return;
		case PiAddress::Status_0x04600010:
			if (value & 2) InterruptLower<Interruption::PI>(n64);
			return;
		case PiAddress::BsdDom1Lat_0x04600014:
			m_bsdDom1Lat = value & 0xFF;
			return;
		case PiAddress::BsdDom1Pwd_0x04600018:
			m_bsdDom1Pwd = value & 0xFF;
			return;
		case PiAddress::BsdDom1Pgs_0x0460001C:
			m_bsdDom1Pgs = value & 0xFF;
			return;
		case PiAddress::BsdDom1Rls_0x04600020:
			m_bsdDom1Rls = value & 0xFF;
			return;
		case PiAddress::BsdDom2Lat_0x04600024:
			m_bsdDom2Lat = value & 0xFF;
			return;
		case PiAddress::BsdDom2Pwd_0x04600028:
			m_bsdDom2Pwd = value & 0xFF;
			return;
		case PiAddress::BsdDom2Pgs_0x0460002C:
			m_bsdDom2Pgs = value & 0xFF;
			return;
		case PiAddress::BsdDom2Rls_0x04600030:
			m_bsdDom2Rls = value & 0xFF;
			return;
		default: break;
		}

		N64Logger::Abort(U"unsupported pi write: paddr={:08X}, value={:08X}"_fmt(static_cast<uint32>(paddr), value));
	}
}
