#pragma once
#include "Utils/Util.h"

namespace N64
{
	class PAddr32 : public Utils::EnumValue<uint32>
	{
	public:
		constexpr explicit PAddr32(uint32 addr): EnumValue(addr) { return; }
	};

	namespace Cpu_detail
	{
		class Cpu;
	}

	using Cpu = Cpu_detail::Cpu;

	namespace Memory_detail
	{
		class Memory;
	}

	using Memory = Memory_detail::Memory;

	class N64System;
}
