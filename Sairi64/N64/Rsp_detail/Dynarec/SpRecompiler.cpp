#include "stdafx.h"

#include "N64/N64System.h"

#define RSP_PROCESS_INTERNAL
#include "Decoder.h"
#include "SpRecompiler.h"

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	inline Instruction fetchInstruction(Rsp& rsp, ImemAddr16 pc)
	{
		return ReadBytes32(rsp.Imem(), pc);
	}

	void assembleStepPc(x86::Assembler& x86Asm, const PcRaw& pc)
	{
		x86Asm.mov(x86::rax, (uint64)&pc.curr);
		x86Asm.mov(x86::cx, x86::word_ptr(x86::rax, 0));
		x86Asm.mov(x86::word_ptr(x86::rax, OFFSET_TO(PcRaw, curr, prev)), x86::cx);
		x86Asm.mov(x86::cx, x86::word_ptr(x86::rax, OFFSET_TO(PcRaw, curr, next)));
		x86Asm.mov(x86::word_ptr(x86::rax, 0), x86::cx);
		x86Asm.add(x86::cx, 4);
		x86Asm.and_(x86::cx, SpPcMask_0xFFC);
		x86Asm.mov(x86::word_ptr(x86::rax, OFFSET_TO(PcRaw, curr, next)), x86::cx);
	}

	uint32 assembleCodeInternal(const AssembleContext& ctx, ImemAddr16 startPc)
	{
		AssembleState state{
			.recompiledLength = 0,
			.scanPc = startPc,
			.scanningDelaySlot = false
		};
		auto&& rsp = *ctx.rsp;
		auto&& x86Asm = *ctx.x86Asm;

		while (true)
		{
			if (state.scanPc >= SpImemSize_0x1000) break;

			const Instruction fetchedInstr = fetchInstruction(rsp, ImemAddr16(state.scanPc));

			state.recompiledLength += 1;
			state.scanPc += 4;

			assembleStepPc(x86Asm, Process::AccessPc(rsp));

			const DecodedToken decoded = Decoder::AssembleInstr(ctx, state, fetchedInstr);
			if (decoded == DecodedToken::End) break;

			// 遅延スロットのデコードをした後は終了
			if (decoded != DecodedToken::Branch && state.scanningDelaySlot) break;

			// 分岐命令の次回は遅延スロット
			if (decoded == DecodedToken::Branch) state.scanningDelaySlot = true;
		}

		return state.recompiledLength;
	}

	uint16 assembleCode(N64System& n64, Rsp& rsp, ImemAddr16 startPc, x86::Assembler& x86Asm)
	{
		x86Asm.sub(x86::rsp, StackSizeRequirement);

		const AssembleContext ctx{
			.n64 = &n64,
			.rsp = &rsp,
			.x86Asm = &x86Asm,
			.endLabel = x86Asm.newLabel()
		};
		const uint16 recompiledLength = assembleCodeInternal(ctx, startPc);

		x86Asm.mov(x86::rax, recompiledLength);
		x86Asm.bind(ctx.endLabel); // @end
		x86Asm.add(x86::rsp, StackSizeRequirement);
		x86Asm.ret();

		return recompiledLength;
	}

	SpRecompileResult SpRecompileFreshCode(N64System& n64, Rsp& rsp, ImemAddr16 startPc)
	{
		SpRecompileResult result{};
		asmjit::CodeHolder code{};
		auto&& jit = n64.GetJit();
		code.init(jit.environment());
		x86::Assembler x86Asm(&code);

		result.recompiledLength = assembleCode(n64, rsp, startPc, x86Asm);

		const uint32 error = jit.add(&result.code, &code);
		if (error != 0)
		{
			N64Logger::Abort(U"failed to rsp-recompile: error={}"_fmt(error));
		}
		return result;
	}
}
