#pragma once
#include "N64/Forward.h"

namespace N64::Rdp_detail
{
	using namespace Utils;

	class DpcStatusWrite32
	{
	public:
		DpcStatusWrite32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto ClearXbusDmemDma() { return BitAccess<0>(m_raw); } // 1
		auto SetXbusDmemDma() { return BitAccess<1>(m_raw); } // 1
		auto ClearFreeze() { return BitAccess<2>(m_raw); } // 1
		auto SetFreeze() { return BitAccess<3>(m_raw); } // 1
		auto ClearFlush() { return BitAccess<4>(m_raw); } // 1
		auto SetFlush() { return BitAccess<5>(m_raw); } // 1
		auto ClearTmem() { return BitAccess<6>(m_raw); } // 1
		auto ClearPipe() { return BitAccess<7>(m_raw); } // 1
		auto ClearCmd() { return BitAccess<8>(m_raw); } // 1
		auto ClearClock() { return BitAccess<9>(m_raw); } // 1
		// [10, 31] 22
	private:
		uint32 m_raw{};
	};

	class DpcStatus32
	{
	public:
		DpcStatus32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto XbusDmemDma() { return BitAccess<0>(m_raw); } // 1
		auto Freeze() { return BitAccess<1>(m_raw); } // 1
		auto Flush() { return BitAccess<2>(m_raw); } // 1
		auto StartGclk() { return BitAccess<3>(m_raw); } // 1
		auto TmemBusy() { return BitAccess<4>(m_raw); } // 1
		auto PipeBusy() { return BitAccess<5>(m_raw); } // 1
		auto CmdBusy() { return BitAccess<6>(m_raw); } // 1
		auto CbufReady() { return BitAccess<7>(m_raw); } // 1
		auto DmaBusy() { return BitAccess<8>(m_raw); } // 1
		auto EndValid() { return BitAccess<9>(m_raw); } // 1
		auto StartValid() { return BitAccess<10>(m_raw); } // 1
		// [11, 31] 21
	private:
		uint32 m_raw{};
	};

	struct Dpc
	{
		DpcStatus32 status;
		uint32 start;
		uint32 current;
		uint32 end;
		uint32 clock;
		uint32 tmem;
	};
}
