#include "stdafx.h"
#include "Cpu.h"

#include "Cpu_Interpreter.h"
#include "Instruction.h"
#include "N64/Interrupt.h"
#include "N64/Mmu.h"
#include "N64/N64Logger.h"
#include "Utils/Util.h"

class N64::Cpu_detail::Cpu::Impl
{
public:
	template <ProcessorType processor>
	static void updateCountAndCheckCompareInterrupt(
		N64System& n64, Cpu& cpu,
		std::conditional_t<processor == ProcessorType::Interpreter, void*, CpuCycles> cycles)
	{
		auto&& cop0Reg = cpu.GetCop0().Reg();
		if constexpr (processor == ProcessorType::Interpreter)
		{
			cop0Reg.count += 1; // インタプリタ型は1ステップずつカウント
			cop0Reg.count &= 0x1FFFFFFFF;
			if (cop0Reg.count == (static_cast<uint64>(cop0Reg.compare) << 1))
			{
				// 割り込み発生
				cop0Reg.cause.Ip7().Set(true);
				UpdateInterrupt(n64);
			}
		}
		else if constexpr (processor == ProcessorType::Dynarec)
		{
			const uint64 before = cop0Reg.count;
			const uint64 after = cop0Reg.count + cycles;
			const uint64 compare = static_cast<uint64>(cop0Reg.compare) << 1;
			if (before < compare && compare <= after)
			{
				// 割り込み発生
				cop0Reg.cause.Ip7().Set(true);
				UpdateInterrupt(n64);
			}
			cop0Reg.count = after & 0x1FFFFFFFF;
		}
	}

	static bool shouldServiceInterrupt(const Cop0& cop0)
	{
		auto&& cop0Reg = cop0.Reg();
		auto status = Cop0Status32(cop0Reg.status);

		const bool interruptsPending =
			(status.Im() & Cop0Cause32(cop0Reg.cause).InterruptPending()) != 0;
		const bool interruptsEnabled = status.Ie();
		const bool currentlyHandlingException = status.Exl();
		const bool currentlyHandlingError = status.Erl();
		return interruptsPending && interruptsEnabled &&
			(currentlyHandlingException == false) && (currentlyHandlingError == false);
	}

	static CpuCycles takeDynarecCycle(N64System& n64, Cpu& cpu)
	{
		// check for interrupt/exception
		if (shouldServiceInterrupt(cpu.m_cop0))
		{
			cpu.handleException(cpu.m_pc.Curr(), ExceptionKinds::Interrupt, 0);
			return 1;
		}

		// instruction fetch
		const Optional<PAddr32> paddrOfPc = Mmu::ResolveVAddr(cpu, cpu.m_pc.Curr());
		if (paddrOfPc.has_value() == false)
		{
			cpu.m_cop0.HandleTlbException(cpu.m_pc.Curr());
			cpu.handleException(cpu.m_pc.Curr(), cpu.m_cop0.GetTlbExceptionCode<BusAccess::Load>(), 0);
			return 1;
		}

		const auto code = cpu.m_dynarec.cache.HitBlockCodeOrRecompile(n64, cpu, paddrOfPc.value());
		return code();
	}
};

namespace N64::Cpu_detail
{
	// インタプリタ実行
	void Cpu::stepInterpreter(N64System& n64)
	{
		N64_TRACE(U"cpu cycle starts pc={:#018x}"_fmt(m_pc.Curr()));

		// update delay slot
		m_delaySlot.Step();

		// check for interrupt/exception
		if (Impl::shouldServiceInterrupt(m_cop0))
		{
			handleException(m_pc.Curr(), ExceptionKinds::Interrupt, 0);
			return;
		}

		// instruction fetch
		const Optional<PAddr32> paddrOfPc = Mmu::ResolveVAddr(*this, m_pc.Curr());
		if (paddrOfPc.has_value() == false)
		{
			m_cop0.HandleTlbException(m_pc.Curr());
			handleException(m_pc.Curr(), m_cop0.GetTlbExceptionCode<BusAccess::Load>(), 0);
			return;
		}
		const Instruction fetchedInstr = {Mmu::ReadPaddr32(n64, paddrOfPc.value())};
		N64_TRACE(U"fetched instr={:08X} from pc={:016X}"_fmt(fetchedInstr.Raw(), m_pc.Curr()));

		// update pc
		m_pc.Step();

		// execution
		Interpreter::InterpretInstruction(n64, *this, fetchedInstr);

		// check compare interrupt
		Impl::updateCountAndCheckCompareInterrupt<ProcessorType::Interpreter>(n64, *this, nullptr);
	}

	// 動的再コンパイラ実行
	CpuCycles Cpu::stepDynarec(N64System& n64)
	{
		N64_TRACE(U"cpu cycle starts pc={:#018x}"_fmt(m_pc.Curr()));

		const CpuCycles taken = Impl::takeDynarecCycle(n64, *this);

		// check compare interrupt
		Impl::updateCountAndCheckCompareInterrupt<ProcessorType::Dynarec>(n64, *this, taken);

		return taken;
	}

	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/r4300i.c#L68
	void Cpu::handleException(uint64 pc, ExceptionCode code, int coprocessorError)
	{
		const bool oldExl = m_cop0.Reg().status.Exl();

		N64_TRACE(U"exception thrown: pc={:16X}, code={}, coprocessor={}"_fmt(
			pc, static_cast<uint32>(code), coprocessorError));

		if (oldExl == false)
		{
			if (m_delaySlot.Prev())
			{
				m_cop0.Reg().cause.BranchDelay().Set(true);
				pc -= 4;
			}
			else
			{
				m_cop0.Reg().cause.BranchDelay().Set(false);
			}

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
