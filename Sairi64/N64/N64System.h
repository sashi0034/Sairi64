#pragma once
#include "Cpu.h"
#include "Memory.h"

namespace N64
{
	class N64System
	{
	public:
		Cpu& GetCpu() { return m_cpu; }
		Memory& GetMemory() { return m_memory; }

	private:
		Cpu m_cpu;
		Memory m_memory;
	};
}
