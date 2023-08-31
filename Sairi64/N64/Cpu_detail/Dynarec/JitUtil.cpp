#include "stdafx.h"

#include "JitUtil.h"
#include "N64/N64Logger.h"

namespace N64::Cpu_detail::Dynarec
{
	void debugBreakPoint(N64System& n64)
	{
		N64_TRACE(U"break point! n64={:16X}"_fmt(reinterpret_cast<uint64>(&n64)));
	}

	void CallBreakPoint(const AssembleContext& ctx)
	{
		auto&& x86Asm = *ctx.x86Asm;
		x86Asm.mov(x86::rcx, ctx.n64);
		x86Asm.mov(x86::rax, &debugBreakPoint);
		x86Asm.call(x86::rax);
	}
}