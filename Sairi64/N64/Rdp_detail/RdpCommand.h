#pragma once
#include "Utils/Util.h"

namespace N64::Rdp_detail
{
	using namespace Utils;

	inline constexpr std::array<uint8, 64> RdpCommandLengthData = {
		2, 2, 2, 2, 2, 2, 2, 2, 8, 12, 24, 28, 24, 28, 40, 44,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
	};

	constexpr uint8 RdpCommandFullSync_0x29 = 0x29;

	class RdpCommand
	{
	public:
		uint8 CommandId() const { return GetBits<56, 61>(m_span[0]); }

		static RdpCommand MakeWithRearrangeWords(std::span<uint32> span32);

	private:
		explicit RdpCommand(std::span<uint64> span): m_span(span) { return; }

		std::span<uint64> m_span{};
	};
}
