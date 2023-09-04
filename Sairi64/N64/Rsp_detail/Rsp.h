#pragma once
#include "N64/Forward.h"
#include "RspAddress.h"
#include "SpMemory.h"
#include "Dynarec/ImemCache.h"

namespace N64::Rsp_detail
{
	struct PcRaw
	{
		uint16 prev{};
		uint16 curr{};
		uint16 next{};
	};

	class Pc
	{
	public:
		uint16 Curr() const { return m_raw.curr; }
		uint16 Next() const { return m_raw.next; }
		void SetNext(uint16 next) { m_raw.next = next; }

		void Step()
		{
			m_raw.prev = m_raw.curr;
			m_raw.curr = m_raw.next;
			m_raw.next += 4;
		}

		void Reset(uint16 pc)
		{
			m_raw.prev = pc & 0xFFC;
			m_raw.curr = pc & 0xFFC;
			m_raw.next = m_raw.curr + 4;
		}

		PcRaw& Raw() { return m_raw; }

	private:
		PcRaw m_raw{};
	};

	constexpr int GprSize_32 = 32;

	class Gpr
	{
	public:
		uint32 Read(uint32 index) const { return m_reg[index]; }
		void Write(uint32 index, uint32 value) { if (index != 0) m_reg[index] = value; }
		std::array<uint32, GprSize_32>& Raw() { return m_reg; };
		const std::array<uint32, GprSize_32>& Raw() const { return m_reg; };

	private:
		std::array<uint32, GprSize_32> m_reg{};
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

		Gpr& GetGpr() { return m_gpr; }
		SpStatus32& Status() { return m_status; }

		template <typename Wire> Wire ReadDmem(uint32 addr) { return Utils::ReadBytes<Wire>(m_dmem, addr); }

		class Process;

	private:
		class Interface;

		SpDmem m_dmem{};
		SpImem m_imem{};
		Dynarec::ImemCache m_imemCache{};

		Pc m_pc{};
		Gpr m_gpr{};
		SpDmaLength32 m_dmaLength{};
		SpStatus32 m_status{};
		bool m_semaphore{};
	};
}
