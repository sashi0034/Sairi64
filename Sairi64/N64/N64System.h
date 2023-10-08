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
	class N64System : Uncopyable
	{
	public:
		Cpu& GetCpu() { return m_cpu; }
		const Cpu& GetCpu() const { return m_cpu; }

		Rsp& GetRsp() { return m_rsp; }
		const Rsp& GetRsp() const { return m_rsp; }

		Rdp& GetRdp() { return m_rdp; }
		const Rdp& GetRdp() const { return m_rdp; }

		Memory& GetMemory() { return m_memory; }
		const Memory& GetMemory() const { return m_memory; }

		Mmio::SI& GetSI() { return m_si; }
		const Mmio::SI& GetSI() const { return m_si; }

		Mmio::RI& GetRI() { return m_ri; }
		const Mmio::RI& GetRI() const { return m_ri; }

		Mmio::PI& GetPI() { return m_pi; }
		const Mmio::PI& GetPI() const { return m_pi; }

		Mmio::MI& GetMI() { return m_mi; }
		const Mmio::MI& GetMI() const { return m_mi; }

		Mmio::AI& GetAI() { return m_ai; }
		const Mmio::AI& GetAI() const { return m_ai; }

		Mmio::VI& GetVI() { return m_vi; }
		const Mmio::VI& GetVI() const { return m_vi; }

		Scheduler& GetScheduler() { return m_scheduler; }
		const Scheduler& GetScheduler() const { return m_scheduler; }

		asmjit::JitRuntime& GetJit() { return m_jit; }
		const asmjit::JitRuntime& GetJit() const { return m_jit; }

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
