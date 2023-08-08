#pragma once

namespace N64
{
	class N64System;
}

namespace N64::Cpu_detail
{
	class Pc
	{
	public:
		uint64 Curr() const { return m_curr; }
		uint64 Next() const { return m_next; }
		void SetNext(uint64 next) { m_next = next; }

		void Step()
		{
			m_curr = m_next;
			m_next += 4;
		}

		void Reset(uint64 pc)
		{
			m_curr = pc;
			m_next = pc + 4;
		}

	private:
		uint64 m_curr{};
		uint64 m_next{};
	};

	class Cpu
	{
	public:
		void Step(N64System& sys);
	private:
		Pc m_pc;
	};
}
