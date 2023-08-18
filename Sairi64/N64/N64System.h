﻿#pragma once
#include "Cpu.h"
#include "Memory.h"
#include "Rsp.h"
#include "Scheduler.h"
#include "Mmio/PI.h"
#include "Mmio/RI.h"
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
		Mmio::RI& GetRI() { return m_ri; }
		Mmio::PI& GetPI() { return m_pi; }
		Scheduler& GetScheduler() { return m_scheduler; }

		N64System& operator=(const N64System&) = delete;

	private:
		Cpu m_cpu{};
		Rsp m_rsp{};
		Memory m_memory{};

		Mmio::SI m_si{};
		Mmio::RI m_ri{};
		Mmio::PI m_pi{};

		Scheduler m_scheduler{};
	};
}
