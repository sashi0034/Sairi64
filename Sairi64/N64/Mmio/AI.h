#pragma once
#include "N64/Forward.h"

namespace N64::Mmio
{
	namespace AiAddress
	{
		constexpr PAddr32 DramAddr_0x04500000{0x04500000};
		constexpr PAddr32 Length_0x04500004{0x04500004};
		constexpr PAddr32 Control_0x04500008{0x04500008};
		constexpr PAddr32 Status_0x0450000C{0x0450000C};
		constexpr PAddr32 DacRate_0x04500010{0x04500010};
		constexpr PAddr32 BitRate_0x04500014{0x04500014};
	}

	// https://github.com/SimoneN64/Kaizen/blob/d0bccfc7e7c0d6eaa3662e8286b9d2bf5888b74f/src/backend/core/mmio/AI.hpp#L9
	// https://n64brew.dev/wiki/Audio_Interface
	// Audio Interface
	class AI
	{
	public:
		uint32 Read32(PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);
		// TODO: Step関数

	private:
		bool m_dmaEnable{};
		uint16 m_dacRate{};
		uint8 m_bitRate{};
		int m_dmaCount{};
		std::array<uint32, 2> m_dmaLength{};
		std::array<uint32, 2> m_dmaAddr{};
		bool m_dmaAddrCarry{};
		int m_cycles{};

		struct
		{
			uint32 freq{44100};
			uint32 period{CpuFreq_93750000 / freq};
			uint32 precision{16};
		} m_dac;

		void writeDacRate(N64System& n64, uint32 value);
	};
}
