#pragma once
#include "N64/Forward.h"
#include "RspAddress.h"
#include "SpMemory.h"
#include "VU.h"
#include "Dynarec/ImemCache.h"

namespace N64::Rsp_detail
{
	struct PcRaw
	{
		uint16 prev{};
		uint16 curr{};
		uint16 next{4};
	};

	constexpr uint16 SpPcMask_0xFFC = 0xFFC;

	class Pc
	{
	public:
		uint16 Prev() const { return m_raw.prev; }
		uint16 Curr() const { return m_raw.curr; }
		uint16 Next() const { return m_raw.next; }

		void Step()
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = m_raw.next;
			m_raw.next += 4;
		}

		void Reset(uint16 pc)
		{
			m_raw.prev = pc & SpPcMask_0xFFC;
			m_raw.curr = pc & SpPcMask_0xFFC;
			m_raw.next = m_raw.curr + 4;
		}

		PcRaw& Raw() { return m_raw; }

	private:
		PcRaw m_raw{};
	};

	constexpr int GprSize_32 = 32;
	using GprRaw = std::array<uint32, GprSize_32>;

	class Gpr
	{
	public:
		uint32 Read(uint32 index) const { return m_reg[index]; }
		void Write(uint32 index, uint32 value) { if (index != 0) m_reg[index] = value; }
		GprRaw& Raw() { return m_reg; };
		const GprRaw& Raw() const { return m_reg; };

	private:
		GprRaw m_reg{};
	};

	// https://n64brew.dev/wiki/Reality_Signal_Processor
	// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core
	class Rsp
	{
	public:
		Rsp();

		bool IsHalted() const { return SpStatus32(m_status).Halt(); }
		RspCycles Step(N64System& n64);

		uint32 ReadPAddr32(PAddr32 paddr);
		void WritePAddr32(N64System& n64, PAddr32 paddr, uint32 value);

		SpDmem& Dmem() { return m_dmem; }
		SpImem& Imem() { return m_imem; }
		Dynarec::ImemCache& ImemCache() { return m_imemCache; }
		SpStatus32& Status() { return m_status; }
		Pc& GetPc() { return m_pc; }

		const SpDmem& Dmem() const { return m_dmem; }
		const SpImem& Imem() const { return m_imem; }
		const Dynarec::ImemCache& ImemCache() const { return m_imemCache; }
		const SpStatus32& Status() const { return m_status; }
		const Pc& GetPc() const { return m_pc; }

		class Interface;
		class Process;

	private:
		SpDmem m_dmem{};
		SpImem m_imem{};
		Dynarec::ImemCache m_imemCache{};

		Pc m_pc{};
		Gpr m_gpr{};
		VU m_vu{};
		DivIO m_div{};
		SpDmaLength32 m_dmaLength{};
		SpStatus32 m_status{};

		SpDmaDramAddr32 m_dmaDramAddr{};
		SpDmaSpAddr32 m_dmaSpAddr{};
		SpDmaDramAddr32 m_shadowDmaDramAddr{};
		SpDmaSpAddr32 m_shadowDmaSpAddr{};

		bool m_semaphore{};
	};
}
