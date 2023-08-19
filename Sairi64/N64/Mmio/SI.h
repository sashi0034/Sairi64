#pragma once
#include "Pif.h"

namespace N64::Mmio
{
	// https://n64brew.dev/wiki/Serial_Interface
	// Serial Interface
	class SI
	{
	public:
		PifRam& GetPifRam() { return m_pif.Ram(); }

	private:
		Pif m_pif{};
	};
}
