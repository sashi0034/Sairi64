#pragma once
#include "N64/Forward.h"

namespace N64::Mmio
{
	namespace ViAddress
	{
		constexpr PAddr32 Status_0x04400000{0x04400000};
		constexpr PAddr32 Origin_0x04400004{0x04400004};
		constexpr PAddr32 Width_0x04400008{0x04400008};
		constexpr PAddr32 VInterrupt_0x0440000C{0x0440000C};
		constexpr PAddr32 VCurrent_0x04400010{0x04400010};
		constexpr PAddr32 Burst_0x04400014{0x04400014};
		constexpr PAddr32 VSync_0x04400018{0x04400018};
		constexpr PAddr32 HSync_0x0440001C{0x0440001C};
		constexpr PAddr32 HSyncLeap_0x04400020{0x04400020};
		constexpr PAddr32 HVideo_0x04400024{0x04400024};
		constexpr PAddr32 VVideo_0x04400028{0x04400028};
		constexpr PAddr32 VBurst_0x0440002C{0x0440002C};
		constexpr PAddr32 XScale_0x04400030{0x04400030};
		constexpr PAddr32 YScale_0x04400034{0x04400034};
	}

	// https://n64brew.dev/wiki/Video_Interface
	// Video Interface
	class VI
	{
	public:
		uint32 Read32(PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

	private:
		uint32 m_status{};
		uint32 m_origin{};
		uint32 m_width{320};
		uint32 m_vInterrupt{256};
		uint32 m_vCurrent{};
		uint32 m_burst{};
		uint32 m_vSync{};
		uint32 m_hSync{};
		uint32 m_hSyncLeap{};
		uint32 m_hVideo{};
		uint32 m_vVideo{};
		uint32 m_vBurst{};
		uint32 m_xScale{};
		uint32 m_yScale{};

		int m_numHalfLines{262};
		int m_nmFields{1};
		int m_cyclesPerHalfLine{1000};
	};
}
