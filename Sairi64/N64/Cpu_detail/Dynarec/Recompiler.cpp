#include "stdafx.h"
#include "Recompiler.h"

#include "GprMapper.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "N64/N64System.h"
#include "N64/Cpu_detail/Instruction.h"

#define DYNAREC_RECOMPILER_INTERNAL
#include "Jit.h"

namespace N64::Cpu_detail::Dynarec
{
	using interpret = Cpu::Interpreter::Op;
	class Impl;
}

class N64::Cpu_detail::Dynarec::Impl
{
public:
	static EndFlag AssembleInstr(const AssembleContext& ctx, const AssembleState& state, Instruction instr)
	{
		switch (instr.Op())
		{
		case Opcode::SPECIAL:
			return assembleSPECIAL(ctx, state, static_cast<InstructionR>(instr));
		case Opcode::REGIMM:
			return assembleREGIMM(ctx, state, static_cast<InstructionRegimm>(instr));
		case Opcode::J:
			return UseInterpreter(ctx, instr, &interpret::J);
		case Opcode::JAL:
			return UseInterpreter(ctx, instr, &interpret::JAL);
		case Opcode::BEQ:
			return UseInterpreter(ctx, instr, &interpret::JAL);
		case Opcode::BNE:
			return UseInterpreter(ctx, instr, &interpret::BNE);
		case Opcode::BLEZ:
			return UseInterpreter(ctx, instr, &interpret::BLEZ);
		case Opcode::BGTZ:
			return UseInterpreter(ctx, instr, &interpret::BGTZ);
		case Opcode::ADDI:
			return UseInterpreter(ctx, instr, &interpret::ADDI);
		case Opcode::ADDIU:
			return UseInterpreter(ctx, instr, &interpret::ADDIU);
		case Opcode::SLTI:
			return UseInterpreter(ctx, instr, &interpret::SLTI);
		case Opcode::SLTIU:
			return UseInterpreter(ctx, instr, &interpret::SLTIU);
		case Opcode::ANDI:
			return UseInterpreter(ctx, instr, &interpret::ANDI);
		case Opcode::ORI:
			return UseInterpreter(ctx, instr, &interpret::ORI);
		case Opcode::XORI:
			return UseInterpreter(ctx, instr, &interpret::XORI);
		case Opcode::LUI:
			return UseInterpreter(ctx, instr, &interpret::LUI);
		case Opcode::CP0:
			return assembleCP0(ctx, state, static_cast<InstructionCop>(instr));
		case Opcode::CP1:
			return assembleCP1(ctx, state, static_cast<InstructionCop>(instr));
		case Opcode::CP2:
			break;
		case Opcode::CP3:
			break;
		case Opcode::BEQL:
			return UseInterpreter(ctx, instr, &interpret::BEQL);
		case Opcode::BNEL:
			return UseInterpreter(ctx, instr, &interpret::BNEL);
		case Opcode::BLEZL:
			return UseInterpreter(ctx, instr, &interpret::BLEZL);
		case Opcode::BGTZL:
			return UseInterpreter(ctx, instr, &interpret::BGTZL);
		case Opcode::DADDI:
			return UseInterpreter(ctx, instr, &interpret::DADDI);
		case Opcode::DADDIU:
			return UseInterpreter(ctx, instr, &interpret::DADDIU);
		case Opcode::LDL:
			return UseInterpreter(ctx, instr, &interpret::LDL);
		case Opcode::LDR:
			return UseInterpreter(ctx, instr, &interpret::LDR);
		case Opcode::RDHWR:
			break;
		case Opcode::LB:
			return UseInterpreter(ctx, instr, &interpret::LB);
		case Opcode::LH:
			return UseInterpreter(ctx, instr, &interpret::LH);
		case Opcode::LWL:
			break;
		case Opcode::LW:
			return UseInterpreter(ctx, instr, &interpret::LW);
		case Opcode::LBU:
			return UseInterpreter(ctx, instr, &interpret::LBU);
		case Opcode::LHU:
			return UseInterpreter(ctx, instr, &interpret::LHU);
		case Opcode::LWR:
			break;
		case Opcode::LWU:
			return UseInterpreter(ctx, instr, &interpret::LWU);
		case Opcode::SB:
			return UseInterpreter(ctx, instr, &interpret::SB);
		case Opcode::SH:
			return UseInterpreter(ctx, instr, &interpret::SH);
		case Opcode::SWL:
			break;
		case Opcode::SW:
			return UseInterpreter(ctx, instr, &interpret::SW);
		case Opcode::SDL:
			return UseInterpreter(ctx, instr, &interpret::SDL);
		case Opcode::SDR:
			return UseInterpreter(ctx, instr, &interpret::SDR);
		case Opcode::SWR:
			break;
		case Opcode::CACHE:
			return Jit::CACHE(static_cast<InstructionR>(instr));
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

private:
	static EndFlag assembleSPECIAL(const AssembleContext& ctx, const AssembleState& state, InstructionR instr)
	{
		switch (instr.Funct())
		{
		case OpSpecialFunct::SLL:
			return UseInterpreter(ctx, instr, &interpret::SLL);
		case OpSpecialFunct::SRL:
			return UseInterpreter(ctx, instr, &interpret::SRL);
		case OpSpecialFunct::SRA:
			return UseInterpreter(ctx, instr, &interpret::SRA);
		case OpSpecialFunct::SLLV:
			return UseInterpreter(ctx, instr, &interpret::SLLV);
		case OpSpecialFunct::SRLV:
			return UseInterpreter(ctx, instr, &interpret::SRLV);
		case OpSpecialFunct::SRAV:
			return UseInterpreter(ctx, instr, &interpret::SRAV);
		case OpSpecialFunct::JR:
			return UseInterpreter(ctx, instr, &interpret::JR);
		case OpSpecialFunct::JALR:
			return UseInterpreter(ctx, instr, &interpret::JALR);
		case OpSpecialFunct::SYSCALL:
			break;
		case OpSpecialFunct::SYNC:
			break;
		case OpSpecialFunct::MFHI:
			return UseInterpreter(ctx, instr, &interpret::MFHI);
		case OpSpecialFunct::MTHI:
			return UseInterpreter(ctx, instr, &interpret::MTHI);
		case OpSpecialFunct::MFLO:
			return UseInterpreter(ctx, instr, &interpret::MFLO);
		case OpSpecialFunct::MTLO:
			return UseInterpreter(ctx, instr, &interpret::MTLO);
		case OpSpecialFunct::DSLLV:
			break;
		case OpSpecialFunct::DSRLV:
			break;
		case OpSpecialFunct::DSRAV:
			break;
		case OpSpecialFunct::MULT:
			return UseInterpreter(ctx, instr, &interpret::MULT);
		case OpSpecialFunct::MULTU:
			return UseInterpreter(ctx, instr, &interpret::MULTU);
		case OpSpecialFunct::DIV:
			return UseInterpreter(ctx, instr, &interpret::DIV);
		case OpSpecialFunct::DIVU:
			return UseInterpreter(ctx, instr, &interpret::DIVU);
		case OpSpecialFunct::DMULT:
			break;
		case OpSpecialFunct::DMULTU:
			break;
		case OpSpecialFunct::DDIV:
			break;
		case OpSpecialFunct::DDIVU:
			break;
		case OpSpecialFunct::ADD:
			return Jit::ADDU(ctx, instr); // TODO: オーバーフローハンドリング?
		case OpSpecialFunct::ADDU:
			return Jit::ADDU(ctx, instr);
		case OpSpecialFunct::SUB:
			return UseInterpreter(ctx, instr, &interpret::SUB);
		case OpSpecialFunct::SUBU:
			return UseInterpreter(ctx, instr, &interpret::SUBU);
		case OpSpecialFunct::AND:
			return UseInterpreter(ctx, instr, &interpret::AND);
		case OpSpecialFunct::OR:
			return UseInterpreter(ctx, instr, &interpret::OR);
		case OpSpecialFunct::XOR:
			return UseInterpreter(ctx, instr, &interpret::XOR);
		case OpSpecialFunct::NOR:
			return UseInterpreter(ctx, instr, &interpret::NOR);
		case OpSpecialFunct::SLT:
			return UseInterpreter(ctx, instr, &interpret::SLT);
		case OpSpecialFunct::SLTU:
			return UseInterpreter(ctx, instr, &interpret::SLTU);
		case OpSpecialFunct::DADD:
			return UseInterpreter(ctx, instr, &interpret::DADD);
		case OpSpecialFunct::DADDU:
			return UseInterpreter(ctx, instr, &interpret::DADDU);
		case OpSpecialFunct::DSUB:
			return UseInterpreter(ctx, instr, &interpret::DSUB);
		case OpSpecialFunct::DSUBU:
			return UseInterpreter(ctx, instr, &interpret::DSUBU);
		case OpSpecialFunct::TGE:
			break;
		case OpSpecialFunct::TGEU:
			break;
		case OpSpecialFunct::TLT:
			break;
		case OpSpecialFunct::TLTU:
			break;
		case OpSpecialFunct::TEQ:
			return UseInterpreter(ctx, instr, &interpret::TEQ);
		case OpSpecialFunct::TNE:
			return UseInterpreter(ctx, instr, &interpret::TNE);
		case OpSpecialFunct::DSLL:
			return UseInterpreter(ctx, instr, &interpret::DSLL);
		case OpSpecialFunct::DSRL:
			return UseInterpreter(ctx, instr, &interpret::DSRL);
		case OpSpecialFunct::DSRA:
			return UseInterpreter(ctx, instr, &interpret::DSRA);
		case OpSpecialFunct::DSLL32:
			return UseInterpreter(ctx, instr, &interpret::DSLL32);
		case OpSpecialFunct::DSRL32:
			return UseInterpreter(ctx, instr, &interpret::DSRL32);
		case OpSpecialFunct::DSRA32:
			return UseInterpreter(ctx, instr, &interpret::DSRA32);
		default: ;
		}

		N64Logger::Abort(U"not implemented: instruction {}"_fmt(instr.OpName()));
		return {};
	}

	static EndFlag assembleREGIMM(const AssembleContext& ctx, const AssembleState& state, InstructionRegimm instr)
	{
		switch (instr.Sub())
		{
		case OpRegimm::BLTZ:
			return UseInterpreter(ctx, instr, &interpret::BLTZ);
		case OpRegimm::BGEZ:
			return UseInterpreter(ctx, instr, &interpret::BGEZ);
		case OpRegimm::BLTZL:
			return UseInterpreter(ctx, instr, &interpret::BLTZL);
		case OpRegimm::BGEZL:
			return UseInterpreter(ctx, instr, &interpret::BGEZL);
		case OpRegimm::TGEI:
			break;
		case OpRegimm::TGEIU:
			break;
		case OpRegimm::TLTI:
			break;
		case OpRegimm::TLTIU:
			break;
		case OpRegimm::TEQI:
			break;
		case OpRegimm::TNEI:
			break;
		case OpRegimm::BLTZAL:
			break;
		case OpRegimm::BGEZAL:
			return UseInterpreter(ctx, instr, &interpret::BGEZAL);
		case OpRegimm::BGEZALL:
			return UseInterpreter(ctx, instr, &interpret::BGEZALL);
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	static EndFlag assembleCP0(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
	{
		switch (instr.Sub())
		{
		case OpCopSub::MFC:
			return UseInterpreter(ctx, instr, &interpret::Cop::MFC0);
		case OpCopSub::DMFC:
			return UseInterpreter(ctx, instr, &interpret::Cop::DMFC0);
		case OpCopSub::MTC:
			return UseInterpreter(ctx, instr, &interpret::Cop::MTC0);
		case OpCopSub::DMTC:
			return UseInterpreter(ctx, instr, &interpret::Cop::DMTC0);
		case OpCopSub::CFC:
			break;
		case OpCopSub::CTC:
			break;
		// @formatter:off
		case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
		case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
		case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
		case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F: // @formatter:on
			return assembleCO0(ctx, state, static_cast<InstructionCop0Co>(instr));
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	[[nodiscard]]
	static EndFlag assembleCO0(const AssembleContext& ctx, const AssembleState& state, InstructionCop0Co instr)
	{
		switch (instr.Funct())
		{
		case OpCop0CoFunct::TLBR:
			break;
		case OpCop0CoFunct::TLBWI:
			break;
		case OpCop0CoFunct::TLBWR:
			break;
		case OpCop0CoFunct::TLBP:
			break;
		case OpCop0CoFunct::ERET:
			return UseInterpreter(ctx, instr, &interpret::Cop::ERET);
		default: break;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}

	static EndFlag assembleCP1(const AssembleContext& ctx, const AssembleState& state, InstructionCop instr)
	{
		switch (const auto sub = static_cast<InstructionCopSub>(instr); instr.Sub())
		{
		case OpCopSub::MFC:
			break;
		case OpCopSub::DMFC:
			break;
		case OpCopSub::MTC:
			break;
		case OpCopSub::DMTC:
			break;
		case OpCopSub::CFC:
			return UseInterpreter(ctx, sub, &interpret::Cop::CFC1);
		case OpCopSub::CTC:
			return UseInterpreter(ctx, sub, &interpret::Cop::CTC1);
		// @formatter:off
		case OpCopSub::CO_0x10: case OpCopSub::CO_0x11: case OpCopSub::CO_0x12: case OpCopSub::CO_0x13:
		case OpCopSub::CO_0x14: case OpCopSub::CO_0x15: case OpCopSub::CO_0x16: case OpCopSub::CO_0x17:
		case OpCopSub::CO_0x18: case OpCopSub::CO_0x19: case OpCopSub::CO_0x1A: case OpCopSub::CO_0x1B:
		case OpCopSub::CO_0x1C: case OpCopSub::CO_0x1D: case OpCopSub::CO_0x1E: case OpCopSub::CO_0x1F: // @formatter:on
			// TODO
			break;
		default: ;
		}

		N64Logger::Abort(U"not implemented: {}"_fmt(instr.Stringify()));
		return {};
	}
};

namespace N64::Cpu_detail::Dynarec
{
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
			if (state.recompiledLength > maxRecompilableLength)
			{
				FlashPc(ctx, state.shadowScanPc);
				FlashDelaySlot(ctx, state.scanDelaySlot);
				break;
			}

			// 命令フェッチ
			const Instruction fetchedInstr = {Mmu::ReadPaddr32(*ctx.n64, PAddr32(state.scanPc))};

			state.recompiledLength += 1;
			state.scanPc += 4;
			state.shadowScanPc.Step();
			state.scanDelaySlot.Step();

			// TODO: 命令内でフレッシュするか判断?
			FlashPc(ctx, state.shadowScanPc);
			FlashDelaySlot(ctx, state.scanDelaySlot);

			// 命令アセンブル
			const EndFlag end = Impl::AssembleInstr(ctx, state, fetchedInstr);
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
