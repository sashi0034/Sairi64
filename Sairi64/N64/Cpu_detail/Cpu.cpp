#include "stdafx.h"
#include "Cpu.h"

#include "Cpu_Interpreter.h"
#include "Instruction.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	void Cpu::Step(N64System& n64)
	{
		N64_TRACE(U"cpu cycle starts pc={:#018x}"_fmt(m_pc.Curr()));

		const Optional<PAddr32> paddrOfPc = Mmu::ResolveVAddr(*this, m_pc.Curr());
		if (paddrOfPc.has_value() == false)
		{
			// TODO
			N64Logger::Abort();
		}

		const Instruction fetchedInstr = {Mmu::ReadPaddr32(n64, paddrOfPc.value())};
		N64_TRACE(U"fetched instr={:08X} from pc={:016X}"_fmt(fetchedInstr.Raw(), m_pc.Curr()));

		m_pc.Step();

		Interpreter::InterpretInstruction(n64, *this, fetchedInstr);
	}

	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/r4300i.c#L68
	void Cpu::handleException(uint64 pc, ExceptionCode code, int coprocessorError)
	{
		const bool oldExl = m_cop0.Reg().status.Exl();

		N64_TRACE(U"exception thrown: pc={:16X}, code={}, coprocessor={}"_fmt(
			pc, static_cast<uint32>(code), coprocessorError));

		if (oldExl != 0)
		{
			// TODO: 分岐関連を実装してから
			// ...

			m_cop0.Reg().epc = pc;
			m_cop0.Reg().status.Exl().Set(true);
		}

		m_cop0.Reg().cause.ExceptionCode().Set(code);
		m_cop0.Reg().cause.CoprocessorError().Set(coprocessorError);

		if (m_cop0.Reg().status.Bev())
		{
			N64Logger::Abort(U"exception {} with bev"_fmt(static_cast<uint32>(code)));
		}
		else
		{
			switch (code)
			{
			case ExceptionKinds::Interrupt:
			case ExceptionKinds::TLBModification:
			case ExceptionKinds::TLBMissLoad:
			case ExceptionKinds::TLBMissStore:
			case ExceptionKinds::BusErrorInstructionFetch:
			case ExceptionKinds::BusErrorLoadStore:
			case ExceptionKinds::Syscall:
			case ExceptionKinds::Breakpoint:
			case ExceptionKinds::ReservedInstruction:
			case ExceptionKinds::CoprocessorUnusable:
			case ExceptionKinds::ArithmeticOverflow:
			case ExceptionKinds::Trap:
			case ExceptionKinds::FloatingPoint:
			case ExceptionKinds::Watch:
				m_pc.Change32(0x80000180);
				break;

			case ExceptionKinds::AddressErrorLoad:
			case ExceptionKinds::AddressErrorStore:
				// TODO: TLBエラーなどをつくってから
				// ...
				break;

			default: break;
			}
		}

		// TODO: CP0更新など?
	}
}
