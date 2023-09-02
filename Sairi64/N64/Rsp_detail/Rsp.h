#pragma once
#include "RspAddress.h"
#include "N64/Forward.h"

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

	private:
		PcRaw m_raw{};
	};

	constexpr uint32 SpDmemSize_0x1000 = 0x1000;
	constexpr uint32 SpImemSize_0x1000 = 0x1000;

	using SpDmem = std::array<uint8, SpDmemSize_0x1000>;
	using SpImem = std::array<uint8, SpImemSize_0x1000>;

	// https://n64brew.dev/wiki/Reality_Signal_Processor
	// https://n64brew.dev/wiki/Reality_Signal_Processor/CPU_Core
	class Rsp
	{
	public:
		Rsp();

		void Step(N64System& n64);

		uint32 ReadPAddr32(PAddr32 paddr);
		void WritePAddr32(N64System& n64, PAddr32 paddr, uint32 value);

		SpDmem& Dmem() { return m_dmem; }
		SpImem& Imem() { return m_imem; }

		template <typename Wire> Wire ReadDmem(uint32 addr) { return Utils::ReadBytes<Wire>(m_dmem, addr); }

	private:
		class Interface;

		SpDmem m_dmem{};
		SpImem m_imem{};

		Pc m_pc{};
		SpDmaLength32 m_dmaLength{};
		SpStatus m_status{};
		bool m_semaphore{};
	};
}
