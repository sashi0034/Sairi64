#pragma once
#include "Cpu.h"
#include "Memory.h"
#include "Rdp.h"
#include "Rsp.h"
#include "Scheduler.h"
#include "Mmio/AI.h"
#include "Mmio/MI.h"
#include "Mmio/PI.h"
#include "Mmio/RI.h"
#include "Mmio/SI.h"
#include "Mmio/VI.h"

namespace N64
{
	class N64System
	{
	public:
		Cpu& GetCpu() { return m_cpu; }
		Rsp& GetRsp() { return m_rsp; }
		Rdp& GetRdp() { return m_rdp; }
		Memory& GetMemory() { return m_memory; }
		Mmio::SI& GetSI() { return m_si; }
		Mmio::RI& GetRI() { return m_ri; }
		Mmio::PI& GetPI() { return m_pi; }
		Mmio::MI& GetMI() { return m_mi; }
		Mmio::AI& GetAI() { return m_ai; }
		Mmio::VI& GetVI() { return m_vi; }
		Scheduler& GetScheduler() { return m_scheduler; }
		asmjit::JitRuntime& GetJit() { return m_jit; }

		N64System& operator=(const N64System&) = delete;

	private:
		Cpu m_cpu{};
		Rsp m_rsp{};
		Rdp m_rdp{};
		Memory m_memory{};

		Mmio::SI m_si{};
		Mmio::RI m_ri{};
		Mmio::PI m_pi{};
		Mmio::MI m_mi{};
		Mmio::AI m_ai{};
		Mmio::VI m_vi{};

		Scheduler m_scheduler{};
		asmjit::JitRuntime m_jit{};
	};
}
