#pragma once
#include "N64/Forward.h"

namespace N64::Mmio
{
	namespace PiAddress
	{
		constexpr PAddr32 DramAddr_0x04600000{0x04600000};
		constexpr PAddr32 CartAddr_0x04600004{0x04600004};
		constexpr PAddr32 RdLen_0x04600008{0x04600008};
		constexpr PAddr32 WrLen_0x0460000C{0x0460000C};
		constexpr PAddr32 Status_0x04600010{0x04600010};
		constexpr PAddr32 BsdDom1Lat_0x04600014{0x04600014};
		constexpr PAddr32 BsdDom1Pwd_0x04600018{0x04600018};
		constexpr PAddr32 BsdDom1Pgs_0x0460001C{0x0460001C};
		constexpr PAddr32 BsdDom1Rls_0x04600020{0x04600020};
		constexpr PAddr32 BsdDom2Lat_0x04600024{0x04600024};
		constexpr PAddr32 BsdDom2Pwd_0x04600028{0x04600028};
		constexpr PAddr32 BsdDom2Pgs_0x0460002C{0x0460002C};
		constexpr PAddr32 BsdDom2Rls_0x04600030{0x04600030};
	}

	class PiStatus32
	{
	public:
		PiStatus32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto DmaBusy() { return Utils::BitAccess<0>(m_raw); }
		auto IoBusy() { return Utils::BitAccess<1>(m_raw); }
		auto DmaError() { return Utils::BitAccess<2>(m_raw); }
		auto Interrupt() { return Utils::BitAccess<3>(m_raw); }

	private:
		uint32 m_raw{};
	};

	// https://n64brew.dev/wiki/Peripheral_Interface
	// Peripheral Interface
	class PI
	{
	public:
		uint32 Read32(N64System& n64, PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

	private:
		uint32 m_dramAddr{};
		uint32 m_cartAddr{};
		uint32 m_rdLen{};
		uint32 m_wrLen{};
		PiStatus32 m_status{};
		uint32 m_bsdDom1Lat{};
		uint32 m_bsdDom1Pwd{};
		uint32 m_bsdDom1Pgs{};
		uint32 m_bsdDom1Rls{};
		uint32 m_bsdDom2Lat{};
		uint32 m_bsdDom2Pwd{};
		uint32 m_bsdDom2Pgs{};
		uint32 m_bsdDom2Rls{};

		void dmaWrite(N64System& n64, uint32 wrLen);
	};
}
