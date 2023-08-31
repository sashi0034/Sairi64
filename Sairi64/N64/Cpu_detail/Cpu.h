#pragma once
#include "Cop0.h"
#include "Cop1.h"
#include "Gpr.h"
#include "Dynarec/RecompiledCache.h"

namespace N64
{
	class N64System;
}

namespace N64::Cpu_detail
{
	struct PcRaw
	{
		uint64 prev{};
		uint64 curr{};
		uint64 next{4};
	};

	class Pc
	{
	public:
		uint64 Prev() const { return m_raw.prev; }
		uint64 Curr() const { return m_raw.curr; }
		uint64 Next() const { return m_raw.next; }
		void SetNext(uint64 next) { m_raw.next = next; }

		void Step()
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = m_raw.next;
			m_raw.next += 4;
		}

		void Change32(uint32 pc)
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = static_cast<sint64>(static_cast<sint32>(pc));
			m_raw.next = m_raw.curr + 4;
		}

		void Change64(uint64 pc)
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = pc;
			m_raw.next = pc + 4;
		}

		PcRaw& Raw() { return m_raw; }

	private:
		PcRaw m_raw;
	};

	struct DelaySlotRow
	{
		bool prev;
		bool curr;
	};

	class DelaySlot
	{
	public:
		bool Prev() const { return m_raw.prev; }
		bool Curr() const { return m_raw.curr; }

		void Step()
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = false;
		}

		void Set() { m_raw.curr = true; }

		DelaySlotRow& Raw() { return m_raw; };

	private:
		DelaySlotRow m_raw{};
	};

	// https://ultra64.ca/files/documentation/silicon-graphics/SGI_R4300_RISC_Processor_Specification_REV2.2.pdf
	class Cpu
	{
	public:
		template <ProcessorType processor> CpuCycles Step(N64System& n64);

		Pc& GetPc() { return m_pc; }
		DelaySlot& GetDelaySlot() { return m_delaySlot; }
		Gpr& GetGpr() { return m_gpr; }
		Cop0& GetCop0() { return m_cop0; }
		Cop1& GetCop1() { return m_cop1; }

		uint64 Lo() const { return m_lo; }
		uint64 Hi() const { return m_hi; }
		void SetLo(uint64 lo) { m_lo = lo; }
		void SetHi(uint64 hi) { m_hi = hi; }

		Dynarec::RecompiledCache& RecompiledCache() { return m_recompiledCache; }

		class Interpreter;

	private:
		class Impl;

		Pc m_pc{};
		DelaySlot m_delaySlot{};
		Gpr m_gpr{};
		Cop0 m_cop0{};
		Cop1 m_cop1{};

		uint64 m_lo{}; // 64ビットの整数乗算/除算レジスタの上位結果
		uint64 m_hi{}; // 64ビットの整数乗算/除算レジスタの下位結果

		Dynarec::RecompiledCache m_recompiledCache{};

		void stepInterpreter(N64System& n64);
		CpuCycles stepDynarec(N64System& n64);

		void handleException(uint64 pc, ExceptionCode code, int coprocessorError);
	};

	template <ProcessorType processor>
	CpuCycles Cpu::Step(N64System& n64)
	{
		if constexpr (processor == ProcessorType::Interpreter)
		{
			stepInterpreter(n64);
			return 1;
		}
		else if constexpr (processor == ProcessorType::Dynarec)
		{
			return stepDynarec(n64);
		}
		else
		{
			static_assert(Utils::AlwaysFalseValue<ProcessorType, processor>);
			return {};
		}
	}
}
