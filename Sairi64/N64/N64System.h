#pragma once
#include "Cpu.h"

namespace N64
{
	class N64System
	{
	public:
		Cpu& GetCpu() { return m_cpu; }

	private:
		Cpu m_cpu;
	};
}
