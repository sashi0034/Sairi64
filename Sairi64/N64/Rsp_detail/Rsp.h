#pragma once
#include "Utils/Util.h"

namespace N64::Rsp_detail
{
	constexpr uint32 RspDmemSize_0x1000 = 0x1000;
	constexpr uint32 RspImemSize_0x1000 = 0x1000;

	class Pc
	{
	public:
		uint16 Curr() const { return m_curr; }
		uint16 Next() const { return m_next; }
		void SetNext(uint16 next) { m_next = next; }

		void Step()
		{
			m_curr = m_next;
			m_next += 4;
		}

		void Reset(uint16 pc)
		{
			m_curr = pc;
			m_next = pc + 4;
		}

	private:
		uint16 m_curr{};
		uint16 m_next{};
	};

	// https://n64brew.dev/wiki/Reality_Signal_Processor
	class Rsp
	{
	public:
		template <typename Wire> Wire ReadDmem(uint32 addr) { return Utils::ReadBytes<Wire>(m_dmem, addr); }

		template <typename Wire> void WriteDmem(uint32 addr, Wire value)
		{
			return Utils::WriteBytes<Wire>(m_dmem, addr, value);
		}

		template <typename Wire> Wire ReadImem(uint32 addr) { return Utils::ReadBytes<Wire>(m_imem, addr); }

		template <typename Wire> void WriteImem(uint32 addr, Wire value)
		{
			return Utils::WriteBytes<Wire>(m_imem, addr, value);
		}

	private:
		std::array<uint8, RspDmemSize_0x1000> m_dmem{};
		std::array<uint8, RspImemSize_0x1000> m_imem{};

		Pc m_pc{};
	};
}
