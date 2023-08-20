#pragma once
#include "Forward.h"

namespace N64
{
	enum class Interruption : uint8
	{
		VI,
		SI,
		PI,
		AI,
		DP,
		SP
	};

	template <Interruption i>
	constexpr std::false_type InterruptionFalse{};

	template <Interruption interrupt> void InterruptRaise(N64System& n64);
	template <Interruption interrupt> void InterruptLower(N64System& n64);

	void UpdateInterrupt(N64System& n64);

	// instantiate
	template void InterruptRaise<Interruption::VI>(N64System& n64);
	template void InterruptRaise<Interruption::SI>(N64System& n64);
	template void InterruptRaise<Interruption::PI>(N64System& n64);
	template void InterruptRaise<Interruption::AI>(N64System& n64);
	template void InterruptRaise<Interruption::DP>(N64System& n64);
	template void InterruptRaise<Interruption::SP>(N64System& n64);

	template void InterruptLower<Interruption::VI>(N64System& n64);
	template void InterruptLower<Interruption::SI>(N64System& n64);
	template void InterruptLower<Interruption::PI>(N64System& n64);
	template void InterruptLower<Interruption::AI>(N64System& n64);
	template void InterruptLower<Interruption::DP>(N64System& n64);
	template void InterruptLower<Interruption::SP>(N64System& n64);
}
