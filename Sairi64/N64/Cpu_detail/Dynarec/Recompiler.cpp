#include "stdafx.h"
#include "Recompiler.h"

#include "GprMapper.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"
#include "N64/Cpu_detail/Instruction.h"

namespace N64::Cpu_detail::Dynarec
{
	namespace x86 = asmjit::x86;

	struct AssembleContext
	{
		N64System* n64;
		Cpu* cpu;
		GprMapper* gprMapper;
		x86::Assembler* x86Asm;
	};

	struct AssembleState
	{
		uint32 recompiledLength;
		uint32 scanPc;
		Pc shadowScanPc;
		DelaySlot scanDelaySlot;
	};

	using EndFlag = bool;

	class Impl
	{
	public:
		static EndFlag AssembleInstr(const AssembleContext& ctx, Instruction instr, const AssembleState& state)
		{
			switch (instr.Op())
			{
			case Opcode::SPECIAL:
				break;
			case Opcode::REGIMM:
				break;
			case Opcode::J:
				break;
			case Opcode::JAL:
				break;
			case Opcode::BEQ:
				break;
			case Opcode::BNE:
				break;
			case Opcode::BLEZ:
				break;
			case Opcode::BGTZ:
				break;
			case Opcode::ADDI:
				break;
			case Opcode::ADDIU:
				break;
			case Opcode::SLTI:
				break;
			case Opcode::SLTIU:
				break;
			case Opcode::ANDI:
				break;
			case Opcode::ORI:
				break;
			case Opcode::XORI:
				break;
			case Opcode::LUI:
				break;
			case Opcode::CP0:
				break;
			case Opcode::CP1:
				break;
			case Opcode::CP2:
				break;
			case Opcode::CP3:
				break;
			case Opcode::BEQL:
				break;
			case Opcode::BNEL:
				break;
			case Opcode::BLEZL:
				break;
			case Opcode::BGTZL:
				break;
			case Opcode::DADDI:
				break;
			case Opcode::DADDIU:
				break;
			case Opcode::LDL:
				break;
			case Opcode::LDR:
				break;
			case Opcode::RDHWR:
				break;
			case Opcode::LB:
				break;
			case Opcode::LH:
				break;
			case Opcode::LWL:
				break;
			case Opcode::LW:
				break;
			case Opcode::LBU:
				break;
			case Opcode::LHU:
				break;
			case Opcode::LWR:
				break;
			case Opcode::LWU:
				break;
			case Opcode::SB:
				break;
			case Opcode::SH:
				break;
			case Opcode::SWL:
				break;
			case Opcode::SW:
				break;
			case Opcode::SDL:
				break;
			case Opcode::SDR:
				break;
			case Opcode::SWR:
				break;
			case Opcode::CACHE:
				break;
			case Opcode::LL:
				break;
			case Opcode::LWC1:
				break;
			case Opcode::LLD:
				break;
			case Opcode::LDC1:
				break;
			case Opcode::LD:
				break;
			case Opcode::SC:
				break;
			case Opcode::SWC1:
				break;
			case Opcode::SCD:
				break;
			case Opcode::SDC1:
				break;
			case Opcode::SD:
				break;
			default: ;
			}

			N64Logger::Abort(U"not implemented: instruction {}"_fmt(instr.OpName()));
			return {};
		}

		static void FlashPc(const AssembleContext& ctx, const Pc& pc)
		{
			return FlashPc(ctx, pc.Prev(), pc.Curr(), pc.Next());
		}

		static void FlashPc(const AssembleContext& ctx,
		                    uint64 prevPc, uint64 currPc, uint64 nextPc)
		{
			auto&& x86Asm = ctx.x86Asm;
			x86Asm->mov(x86::rax, (uint64)&ctx.cpu->GetPc().Raw().curr);
			x86Asm->mov(x86::qword_ptr(x86::rax, -8), prevPc);
			x86Asm->mov(x86::qword_ptr(x86::rax, 0), currPc);
			x86Asm->mov(x86::qword_ptr(x86::rax, 8), nextPc);
		}

		static void FlashDelaySlot(const AssembleContext& ctx, const DelaySlot& delaySlot)
		{
			auto&& x86Asm = ctx.x86Asm;
			x86Asm->mov(x86::rax, (uint64)&ctx.cpu->GetDelaySlot().Raw().curr);
			x86Asm->mov(x86::qword_ptr(x86::rax, -8), delaySlot.Prev());
			x86Asm->mov(x86::qword_ptr(x86::rax, 0), delaySlot.Curr());
		}

	private:
	};

	uint32 assembleCodeInternal(const AssembleContext& ctx, PAddr32 startPc)
	{
		const uint32 maxRecompilableLength = CachePageOffsetSize_0x400 - GetPageIndex(startPc);

		AssembleState state{
			.recompiledLength = 0,
			.scanPc = startPc,
			.shadowScanPc = ctx.cpu->GetPc(), // copy
			.scanDelaySlot = ctx.cpu->GetDelaySlot(), // copy
		};

		while (true)
		{
			// ページ内のみコンパイル
			if (state.recompiledLength > maxRecompilableLength) break;

			// 命令フェッチ
			const Instruction fetchedInstr = {Mmu::ReadPaddr32(*ctx.n64, PAddr32(state.scanPc))};

			state.recompiledLength += 1;
			state.scanPc += 4;
			state.shadowScanPc.Step();
			Impl::FlashPc(ctx, state.shadowScanPc); // TODO: 命令内でフレッシュするか判断?
			state.scanDelaySlot.Step();
			Impl::FlashDelaySlot(ctx, state.scanDelaySlot);

			// 命令アセンブル
			const EndFlag end = Impl::AssembleInstr(ctx, fetchedInstr, state);
			if (end) break;
		}

		return state.recompiledLength;
	}

	uint32 assembleCode(N64System& n64, Cpu& cpu, PAddr32 startPc, x86::Assembler& x86Asm)
	{
		GprMapper gprMapper{};
		gprMapper.PushNonVolatiles(x86Asm);
		constexpr int stackSize = 40;
		x86Asm.sub(x86::rsp, stackSize);

		const AssembleContext ctx{
			.n64 = &n64,
			.cpu = &cpu,
			.gprMapper = &gprMapper,
			.x86Asm = &x86Asm,
		};
		const uint32 recompiledLength = assembleCodeInternal(ctx, startPc);

		gprMapper.FlushClear(x86Asm, cpu.GetGpr());
		x86Asm.add(x86::rsp, stackSize);
		gprMapper.PopNonVolatiles(x86Asm);
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
		jit.add(&result.code, &code);

		return result;
	}
}
