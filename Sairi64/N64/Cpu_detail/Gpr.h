#pragma once

namespace N64::Cpu_detail
{
	constexpr int GprSize_32 = 32;

	constexpr std::array<StringView, GprSize_32> GprNames = {
		U"R0", U"AT", U"V0", U"V1", U"A0", U"A1", U"A2", U"A3", // 8
		U"T0", U"T1", U"T2", U"T3", U"T4", U"T5", U"T6", U"T7", // 16
		U"S0", U"S1", U"S2", U"S3", U"S4", U"S5", U"S6", U"S7", // 24
		U"T8", U"T9", U"K0", U"K1", U"GP", U"SP", U"FP", U"RA", // 32
	};

	class Gpr
	{
	public:
		uint64 Read(uint64 index) const { return m_reg[index]; }
		void Write(uint64 index, uint64 value) { if (index != 0) m_reg[index] = value; }
		std::array<uint64, GprSize_32>& Raw() { return m_reg; };
		const std::array<uint64, GprSize_32>& Raw() const { return m_reg; };

	private:
		std::array<uint64, GprSize_32> m_reg{};
	};
}
