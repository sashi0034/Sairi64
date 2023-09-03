#pragma once
#include "N64/Forward.h"
#include "Utils/Util.h"

namespace N64::Rsp_detail
{
	using namespace Utils;

	namespace RspAddress
	{
		constexpr PAddr32 DmaSpAddr_0x04040000{0x04040000};
		constexpr PAddr32 DmaRamAddr_0x04040004{0x04040004};
		constexpr PAddr32 DmaRdLen_0x04040008{0x04040008};
		constexpr PAddr32 DmaWrLen_0x0404000C{0x0404000C};
		constexpr PAddr32 DmaWrStatus_0x04040010{0x04040010};
		constexpr PAddr32 DmaFull_0x04040014{0x04040014};
		constexpr PAddr32 DmaBusy_0x04040018{0x04040018};
		constexpr PAddr32 Semaphore_0x0404001C{0x0404001C};
		constexpr PAddr32 Pc_0x04080000{0x04080000};
	}

	constexpr uint32 SpImemSize_0x1000 = 0x1000;
	constexpr uint16 SpImemMask_0xFFF = 0xFFF;

	constexpr uint32 SpDmemSize_0x1000 = 0x1000;

	using RspCycles = int16;

	class ImemAddr16 : public EnumValue<uint16>
	{
	public:
		explicit constexpr ImemAddr16(uint16 v): EnumValue(v) { return; }
	};

	class SpStatus32
	{
	public:
		SpStatus32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto Halt() { return BitAccess<0>(m_raw); } // 1
		auto Broke() { return BitAccess<1>(m_raw); } // 1
		auto DmaBusy() { return BitAccess<2>(m_raw); } // 1
		auto DmaFull() { return BitAccess<3>(m_raw); } // 1
		auto IoFull() { return BitAccess<4>(m_raw); } // 1
		auto SingleStep() { return BitAccess<5>(m_raw); } // 1
		auto InterruptOnBreak() { return BitAccess<6>(m_raw); } // 1
		auto Signal0() { return BitAccess<7>(m_raw); } // 1
		auto Signal1() { return BitAccess<8>(m_raw); } // 1
		auto Signal2() { return BitAccess<9>(m_raw); } // 1
		auto Signal3() { return BitAccess<10>(m_raw); } // 1
		auto Signal4() { return BitAccess<11>(m_raw); } // 1
		auto Signal5() { return BitAccess<12>(m_raw); } // 1
		auto Signal6() { return BitAccess<13>(m_raw); } // 1
		auto Signal7() { return BitAccess<14>(m_raw); } // 1
	private:
		uint32 m_raw{};
	};

	class SpStatusWrite32
	{
	public:
		SpStatusWrite32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto ClearHalt() { return BitAccess<0>(m_raw); } // 1
		auto SetHalt() { return BitAccess<1>(m_raw); } // 1
		auto ClearBroke() { return BitAccess<2>(m_raw); } // 1
		auto ClearInterrupt() { return BitAccess<3>(m_raw); } // 1
		auto SetInterrupt() { return BitAccess<4>(m_raw); } // 1
		auto ClearSStep() { return BitAccess<5>(m_raw); } // 1
		auto SetSStep() { return BitAccess<6>(m_raw); } // 1
		auto ClearInterruptOnBreak() { return BitAccess<7>(m_raw); } // 1
		auto SetInterruptOnBreak() { return BitAccess<8>(m_raw); } // 1
		auto ClearSignal0() { return BitAccess<9>(m_raw); } // 1
		auto SetSignal0() { return BitAccess<10>(m_raw); } // 1
		auto ClearSignal1() { return BitAccess<11>(m_raw); } // 1
		auto SetSignal1() { return BitAccess<12>(m_raw); } // 1
		auto ClearSignal2() { return BitAccess<13>(m_raw); } // 1
		auto SetSignal2() { return BitAccess<14>(m_raw); } // 1
		auto ClearSignal3() { return BitAccess<15>(m_raw); } // 1
		auto SetSignal3() { return BitAccess<16>(m_raw); } // 1
		auto ClearSignal4() { return BitAccess<17>(m_raw); } // 1
		auto SetSignal4() { return BitAccess<18>(m_raw); } // 1
		auto ClearSignal5() { return BitAccess<19>(m_raw); } // 1
		auto SetSignal5() { return BitAccess<20>(m_raw); } // 1
		auto ClearSignal6() { return BitAccess<21>(m_raw); } // 1
		auto SetSignal6() { return BitAccess<22>(m_raw); } // 1
		auto ClearSignal7() { return BitAccess<23>(m_raw); } // 1
		auto SetSignal7() { return BitAccess<24>(m_raw); } // 1
		// [25, 31]
	private:
		uint32 m_raw{};
	};

	class SpDmaLength32
	{
	public:
		SpDmaLength32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto Length() { return BitAccess<0, 11>(m_raw); } // 12
		auto Count() { return BitAccess<12, 19>(m_raw); } // 8
		auto Skip() { return BitAccess<20, 31>(m_raw); } // 12
	private:
		uint32 m_raw{};
	};
}
