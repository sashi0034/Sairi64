#pragma once
#include "Pif.h"

namespace N64::Mmio
{
	namespace SiAddress
	{
		constexpr PAddr32 DramAddr_0x04800000{0x04800000};
		constexpr PAddr32 PifAdRd64B_0x04800004{0x04800004};
		constexpr PAddr32 Unknown_0x04800008{0x04800008};
		constexpr PAddr32 Unknown_0x0480000C{0x0480000C};
		constexpr PAddr32 PifAddrWr64B_0x04800010{0x04800010};
		constexpr PAddr32 Unknown_0x04800014{0x04800014};
		constexpr PAddr32 Status_0x04800018{0x04800018};
		constexpr PAddr32 Unknown_0x0480001C{0x0480001C};
	}

	class SiStatus32
	{
	public:
		SiStatus32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto DmaBusy() { return Utils::BitAccess<0>(m_raw); }
		auto RdBusy() { return Utils::BitAccess<1>(m_raw); }
		auto DmaError() { return Utils::BitAccess<3>(m_raw); }
		auto Interrupt() { return Utils::BitAccess<12>(m_raw); }

	private:
		uint32 m_raw{};
	};

	// https://n64brew.dev/wiki/Serial_Interface
	// Serial Interface
	class SI
	{
	public:
		Pif& GetPif() { return m_pif; }
		PifRam& GetPifRam() { return m_pif.Ram(); }

		uint32 Read32(N64System& n64, PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

	private:
		Pif m_pif{};

		uint32 m_dramAddr{};
		uint32 m_pifAddr{};
		SiStatus32 m_status{};
		int m_dmaRunning{};

		bool dmaBusy() const { return m_dmaRunning > 0; }

		enum class DmaType
		{
			DramToPif,
			PifToDram,
		};

		template <DmaType dma> void startDma(N64System& n64, uint32 pifAddr);
		template <DmaType dma> void achieveDma(N64System& n64, uint32 pifAddr);
	};
}
