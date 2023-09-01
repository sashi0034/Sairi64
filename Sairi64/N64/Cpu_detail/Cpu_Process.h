#pragma once

#include "Cpu.h"
#include "N64/N64Logger.h"

#if (not defined(CPU_INTERPRETER_INTERNAL) && not defined(DYNAREC_RECOMPILER_INTERNAL))
#error "This file is an internal file"
#endif

namespace N64::Cpu_detail
{
	enum class BranchType
	{
		// 常に遅延スロットの命令は実行される
		Normal,
		// 分岐条件成立時のみ、遅延スロットの命令は実行される
		Likely,
	};

	class Cpu::Process
	{
	public:
		template <BranchType branch>
		static void BranchVAddr64(Cpu& cpu, uint64 vaddr, bool condition)
		{
			if (condition)
			{
				cpu.m_delaySlot.Set();
				cpu.m_pc.SetNext(vaddr);
				N64_TRACE(U"branch accepted vaddr={:016X}"_fmt(vaddr));
			}
			else
			{
				if constexpr (branch == BranchType::Normal)
				{
					cpu.m_delaySlot.Set();
				}
				else if constexpr (branch == BranchType::Likely)
				{
					// likelyのときは、遅延スロットを実行しないようにする
					cpu.m_pc.Change64(cpu.m_pc.Curr() + 4);
				}
				N64_TRACE(U"branch not accepted (vaddr={:016X})"_fmt(vaddr));
			}
		}

		template <BranchType branch, bool condition>
		static void StaticBranchVAddr64(Cpu& cpu, uint64 vaddr)
		{
			BranchVAddr64<branch>(cpu, vaddr, condition);
		}

		static void ThrowException(Cpu& cpu, ExceptionCode code, int coprocessorError)
		{
			cpu.handleException(cpu.m_pc.Prev(), code, coprocessorError);
		}
	};
}
