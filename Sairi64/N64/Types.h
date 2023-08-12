#pragma once

namespace N64
{
	// TODO: いらなかったら削除

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

	template <class N64>
	concept I64System = requires(N64 n64)
	{
		{ n64.GetCpu() } -> std::same_as<Cpu&>;
		{ n64.GetMemory() } -> std::same_as<Memory&>;
	};

	class N64System;
}
