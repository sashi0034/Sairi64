#pragma once
#include "N64/Forward.h"

namespace N64::Mmio
{
	namespace ViAddress
	{
		constexpr PAddr32 Control_0x04400000{0x04400000};
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

	using Utils::BitAccess;

	class ViControl32
	{
	public:
		ViControl32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto Type() { return BitAccess<0, 1>(m_raw); } // 2
		auto GammaDitherEnable() { return BitAccess<2>(m_raw); } // 1
		auto GammaEnable() { return BitAccess<3>(m_raw); } // 1
		auto DivotEnable() { return BitAccess<4>(m_raw); } // 1
		auto Reserved_AlwaysOff() { return BitAccess<5>(m_raw); } // 1
		auto Serrate() { return BitAccess<6>(m_raw); } // 1
		auto Reserved_DiagnosticsOnly() { return BitAccess<7>(m_raw); } // 1
		auto AntialiasMode() { return BitAccess<8, 10>(m_raw); } // 3
		// [11, 31] 21
	private:
		uint32 m_raw{};
	};

	// https://n64brew.dev/wiki/Video_Interface
	// Video Interface
	class VI
	{
	public:
		uint32 Read32(PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

		ViControl32 Control() const { return m_control; }
		uint32 Origin() const { return m_origin; }
		uint32 Width() const { return m_width; }
		uint32 VInterrupt() const { return m_vInterrupt; }
		uint32 VCurrent() const { return m_vCurrent; }
		uint32 Burst() const { return m_burst; }
		uint32 VSync() const { return m_vSync; }
		uint32 HSync() const { return m_hSync; }
		uint32 HSyncLeap() const { return m_hSyncLeap; }
		uint32 HVideo() const { return m_hVideo; }
		uint32 VVideo() const { return m_vVideo; }
		uint32 VBurst() const { return m_vBurst; }
		uint32 XScale() const { return m_xScale; }
		uint32 YScale() const { return m_yScale; }

		void SetVCurrent(uint32 value) { m_vCurrent = value; }

		uint32 Swaps() const { return m_swaps; }
		uint32 NumHalfLines() const { return m_numHalfLines; }
		uint32 NumFields() const { return m_numFields; }
		uint32 CyclesPerHalfLine() const { return m_cyclesPerHalfLine; }

	private:
		ViControl32 m_control{};
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

		uint32 m_swaps{};
		uint32 m_numHalfLines{262};
		uint32 m_numFields{1};
		uint32 m_cyclesPerHalfLine{1000};
	};
}
