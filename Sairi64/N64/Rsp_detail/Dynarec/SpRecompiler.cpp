#include "stdafx.h"

#include "N64/N64System.h"

#define DYNAREC_SP_RECOMPILER_INTERNAL
#include "Decoder.h"
#include "SpRecompiler.h"

namespace N64::Rsp_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	uint32 assembleCodeInternal(const AssembleContext& ctx, ImemAddr16 startPc)
	{
		AssembleState state{
			.recompiledLength = 0,
			.scanPc = startPc
		};

		while (true)
		{
			if (state.scanPc >= SpImemSize_0x1000) break;

			state.recompiledLength += 1;
			state.scanPc += 4;

			const Instruction fetchedInstr = {0}; // TODO

			// TODO: PCステップ

			const DecodedToken decoded = Decoder::AssembleInstr(ctx, state, fetchedInstr);
			if (decoded == DecodedToken::End) break;
		}

		return state.recompiledLength;
	}

	uint16 assembleCode(N64System& n64, Rsp& rsp, ImemAddr16 startPc, x86::Assembler& x86Asm)
	{
		constexpr int stackSize = 40;
		x86Asm.sub(x86::rsp, stackSize);

		const AssembleContext ctx{
			.n64 = &n64,
			.rsp = &rsp,
			.x86Asm = &x86Asm,
			.endLabel = x86Asm.newLabel()
		};
		const uint16 recompiledLength = assembleCodeInternal(ctx, startPc);

		x86Asm.mov(x86::rax, recompiledLength);
		x86Asm.bind(ctx.endLabel); // @end
		x86Asm.add(x86::rsp, stackSize);
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
