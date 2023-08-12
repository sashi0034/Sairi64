#pragma once
#include "Cpu.h"
#include "Memory.h"
#include "Rsp.h"
#include "Mmio/SI.h"

namespace N64
{
	class N64System
	{
	public:
		Cpu& GetCpu() { return m_cpu; }
		Rsp& GetRsp() { return m_rsp; }
		Memory& GetMemory() { return m_memory; }
		Mmio::SI& GetSI() { return m_si; }

	private:
		Cpu m_cpu{};
		Rsp m_rsp{};
		Memory m_memory{};
		Mmio::SI m_si{};
	};
}
