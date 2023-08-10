#pragma once
#include "Rom.h"

namespace N64::Memory_detail
{
	class Memory
	{
	public:
		Rom& GetRom() { return m_rom; }

	private:
		Rom m_rom{};
	};
}
