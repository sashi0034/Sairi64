#include "stdafx.h"
#include "Recompiler.h"

#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"
#include "N64/Cpu_detail/Instruction.h"

#define DYNAREC_RECOMPILER_INTERNAL
#include "Decoder.h"

namespace N64::Cpu_detail::Dynarec
{
	void checkContinuousPc(const AssembleContext& ctx)
	{
		auto&& x86Asm = *ctx.x86Asm;
		const auto continuousLabel = x86Asm.newLabel();

		x86Asm.mov(x86::r8, (uint64)&ctx.cpu->GetPc().Raw().curr);
		x86Asm.mov(x86::rax, x86::qword_ptr(x86::r8, 0)); // rax <- pc.curr
		x86Asm.mov(x86::rcx, x86::qword_ptr(x86::r8, OFFSET_TO(PcRaw, curr, prev))); // rcx <- pc.prev
		x86Asm.sub(x86::rax, x86::rcx); // rax <- rax - rcx
		x86Asm.cmp(x86::rax, 4); // if rax is
		x86Asm.je(continuousLabel); // equals to 4 then goto @continuous
		// non-continuous
		x86Asm.mov(x86::rax, 1); // passed cycles <- 1
		x86Asm.jmp(ctx.endLabel); // goto @end
		x86Asm.bind(continuousLabel); // @continuous
	}

	void handleBranchLikely(const AssembleContext& ctx, uint32 currentRecompiledLength)
	{
		auto&& x86Asm = *ctx.x86Asm;
		const auto delaySlotLabel = x86Asm.newLabel();

		x86Asm.mov(x86::al, x86::byte_ptr(reinterpret_cast<uint64>(&ctx.cpu->GetDelaySlot().Raw().curr)));
		x86Asm.test(x86::al, x86::al); // if delaySlot is set
		x86Asm.jne(delaySlotLabel); // then goto @delaySlot
		// case delaySlot is not set
		CallBreakPoint(ctx, 1);
		x86Asm.mov(x86::rax, currentRecompiledLength); // rax <- current steps
		x86Asm.jmp(ctx.endLabel); // goto @end
		x86Asm.bind(delaySlotLabel); // @delaySlot
		CallBreakPoint(ctx, 2);
	}

	uint32 assembleCodeInternal(const AssembleContext& ctx, PAddr32 startPc)
	{
		const uint32 maxRecompilableLength = CachePageOffsetSize_0x400 - GetPageIndex(startPc);

		AssembleState state{
			.recompiledLength = 0,
			.scanPc = startPc,
			.scanningDelaySlot = false
		};

		while (true)
		{
			// ページ内のみコンパイル
			if (state.recompiledLength >= maxRecompilableLength)
			{
				// TODO: flash?
				break;
			}

			// 命令フェッチ
			const Instruction fetchedInstr = {Mmu::ReadPaddr32(*ctx.n64, PAddr32(state.scanPc))};

			state.recompiledLength += 1;
			state.scanPc += 4;

			// TODO: 命令内でフレッシュするか判断?
			AssembleStepPc(ctx);
			AssembleStepDelaySlot(ctx);

			// 命令アセンブル
			const DecodedToken decoded = Decoder::AssembleInstr(ctx, state, fetchedInstr);
			if (decoded == DecodedToken::End) break;

			// 遅延スロットのデコードをした後は終了
			if (decoded != DecodedToken::Branch && state.scanningDelaySlot) break;

			// 分岐命令の次回は遅延スロット
			if (decoded == DecodedToken::Branch) state.scanningDelaySlot = true;
			if (decoded == DecodedToken::BranchLikely)
			{
				state.scanningDelaySlot = true;
				handleBranchLikely(ctx, state.recompiledLength);
			}

			if (state.recompiledLength == 1)
			{
				// TODO: 削除?
				// 先頭の命令が分岐時のDelaySlotの可能性があるので、そのときは終了するようにする
				checkContinuousPc(ctx);
			}
		}

		return state.recompiledLength;
	}

	uint32 assembleCode(N64System& n64, Cpu& cpu, PAddr32 startPc, x86::Assembler& x86Asm)
	{
		constexpr int stackSize = 40;
		x86Asm.sub(x86::rsp, stackSize);

		const AssembleContext ctx{
			.n64 = &n64,
			.cpu = &cpu,
			.x86Asm = &x86Asm,
			.endLabel = x86Asm.newLabel()
		};
		const uint32 recompiledLength = assembleCodeInternal(ctx, startPc);

		x86Asm.mov(x86::rax, recompiledLength);
		x86Asm.bind(ctx.endLabel); // @end
		x86Asm.add(x86::rsp, stackSize);
		x86Asm.ret();
		return recompiledLength;
	}

	RecompiledResult RecompileFreshCode(N64System& n64, Cpu& cpu, PAddr32 startPc)
	{
		RecompiledResult result{};
		asmjit::CodeHolder code{};
		auto&& jit = n64.GetJit();
		code.init(jit.environment());
		x86::Assembler x86Asm(&code);

		result.recompiledLength = assembleCode(n64, cpu, startPc, x86Asm);
		const uint32 error = jit.add(&result.code, &code);
		if (error != 0)
		{
			N64Logger::Abort(U"failed to recompile: error={}"_fmt(error));
		}

		N64_TRACE(U"recompiled length: {}"_fmt(result.recompiledLength));

		return result;
	}
}
