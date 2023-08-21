#pragma once
#include "Cop0.h"
#include "Cop1.h"
#include "Gpr.h"

namespace N64
{
	class N64System;
}

namespace N64::Cpu_detail
{
	class Pc
	{
	public:
		uint64 Prev() const { return m_prev; }
		uint64 Curr() const { return m_curr; }
		uint64 Next() const { return m_next; }
		void SetNext(uint64 next) { m_next = next; }

		void Step()
		{
			m_prev = m_curr;
			m_curr = m_next;
			m_next += 4;
		}

		void Change32(uint32 pc)
		{
			m_prev = m_curr;
			m_curr = (sint64)static_cast<sint32>(pc);
			m_next = pc + 4;
		}

		void Change64(uint64 pc)
		{
			m_prev = m_curr;
			m_curr = pc;
			m_next = pc + 4;
		}

	private:
		uint64 m_prev{};
		uint64 m_curr{};
		uint64 m_next{4};
	};

	class DelaySlot
	{
	public:
		bool Prev() const { return m_prev; }
		bool Curr() const { return m_curr; }

		void Step()
		{
			m_prev = m_curr;
			m_curr = false;
		}

		void Set() { m_curr = true; }

	private:
		bool m_prev{};
		bool m_curr{};
	};

	// https://ultra64.ca/files/documentation/silicon-graphics/SGI_R4300_RISC_Processor_Specification_REV2.2.pdf

	class Cpu
	{
	public:
		void Step(N64System& n64);
		Pc& GetPc() { return m_pc; }
		Gpr& GetGpr() { return m_gpr; }
		Cop0& GetCop0() { return m_cop0; }
		Cop1& GetCop1() { return m_cop1; }

		uint64 Lo() const { return m_lo; }
		uint64 Hi() const { return m_hi; }
		void SetLo(uint64 lo) { m_lo = lo; }
		void SetHi(uint64 hi) { m_hi = hi; }

	private:
		class Interpreter;

		Pc m_pc{};
		DelaySlot m_delaySlot{};
		Gpr m_gpr{};
		Cop0 m_cop0{};
		Cop1 m_cop1{};

		uint64 m_lo{}; // 64ビットの整数乗算/除算レジスタの上位結果
		uint64 m_hi{}; // 64ビットの整数乗算/除算レジスタの下位結果

		void handleException(uint64 pc, ExceptionCode code, int coprocessorError);
	};
}
