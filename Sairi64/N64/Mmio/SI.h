#pragma once

namespace N64::Mmio
{
	constexpr uint32 PifRamSize_0x40 = 0x40;

	using PifRam = std::array<uint8, PifRamSize_0x40>;

	class SI
	{
	public:
		PifRam& GetPifRam() { return m_pifRam; }

	private:
		PifRam m_pifRam{};
	};
}
