#pragma once
#include "N64/Forward.h"

namespace N64::Cpu_detail
{
	class Cop0;

	using Utils::BitAccess;

	// https://n64brew.dev/wiki/COP1#FCSR
	// Floating-point Control Status Register
	class Fcsr32
	{
	public:
		Fcsr32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto RoundingMode() { return BitAccess<0, 1>(m_raw); } // 2
		auto FlagInexactOperation() { return BitAccess<2>(m_raw); } // 1
		auto FlagUnderflow() { return BitAccess<3>(m_raw); } // 1
		auto FlagOverflow() { return BitAccess<4>(m_raw); } // 1
		auto FlagDivisionByZero() { return BitAccess<5>(m_raw); } // 1
		auto FlagInvalidOperation() { return BitAccess<6>(m_raw); } // 1
		auto EnableInexactOperation() { return BitAccess<7>(m_raw); } // 1
		auto EnableUnderflow() { return BitAccess<8>(m_raw); } // 1
		auto EnableOverflow() { return BitAccess<9>(m_raw); } // 1
		auto EnableDivisionByZero() { return BitAccess<10>(m_raw); } // 1
		auto EnableInvalidOperation() { return BitAccess<11>(m_raw); } // 1
		auto CauseInexactOperation() { return BitAccess<12>(m_raw); } // 1
		auto CauseUnderflow() { return BitAccess<13>(m_raw); } // 1
		auto CauseOverflow() { return BitAccess<14>(m_raw); } // 1
		auto CauseDivisionByZero() { return BitAccess<15>(m_raw); } // 1
		auto CauseInvalidOperation() { return BitAccess<16>(m_raw); } // 1
		auto CauseUnimplementedOperation() { return BitAccess<17>(m_raw); } // 1
		// [18, 22] 5
		auto Compare() { return BitAccess<23>(m_raw); } // 1
		auto Fs() { return BitAccess<24>(m_raw); } // 1
		// [25, 31] 7

		auto Flag() { return BitAccess<2, 6>(m_raw); } // 5
		auto Enable() { return BitAccess<7, 11>(m_raw); } // 5
		auto Cause() { return BitAccess<12, 17>(m_raw); } // 6
	private:
		uint32 m_raw{};
	};

	struct Cop1Fcr
	{
		uint32 fcr0;
		Fcsr32 fcr31;
	};

	// Floating-point General Register
	class Fgr64
	{
	public:
		Fgr64(uint64 raw = 0): m_raw(raw) { return; }
		operator uint64() const { return m_raw; }

		uint32 Lo() const { return Utils::GetBits<0, 31>(m_raw); }
		uint32 Hi() const { return Utils::GetBits<32, 63>(m_raw); }

		void SetLo(uint32 value) { m_raw = Utils::SetBits<0, 31>(m_raw, value); }
		void SetHi(uint32 value) { m_raw = Utils::SetBits<32, 63>(m_raw, value); }

	private:
		uint64 m_raw{};
	};

	constexpr int FgrCount_32 = 32;

	template <typename T>
	concept FgrWire = std::same_as<T, uint32> || std::same_as<T, uint64>;

	// https://n64brew.dev/wiki/COP1
	class Cop1
	{
	public:
		void SetFgr32(const Cop0& cop0, uint8 index, uint32 value);
		void SetFgr64(const Cop0& cop0, uint8 index, uint64 value);

		uint32 GetFgr32(const Cop0& cop0, uint8 index) const;
		uint64 GetFgr64(const Cop0& cop0, uint8 index) const;

		template <typename T> void SetFgrBy(const Cop0& cop0, uint8 index, T value);
		template <typename T> T GetFgrBy(const Cop0& cop0, uint8 index) const;

		Cop1Fcr& Fcr() { return m_fcr; }
		const Cop1Fcr& Fcr() const { return m_fcr; }

	private:
		Cop1Fcr m_fcr{};
		std::array<Fgr64, FgrCount_32> m_fgr{};
	};

	template <typename T>
	void Cop1::SetFgrBy(const Cop0& cop0, uint8 index, T value)
	{
		if constexpr (sizeof(T) == 4)
		{
			uint32 raw;
			memcpy(&raw, &value, sizeof(T));
			SetFgr32(cop0, index, raw);
		}
		else if constexpr (sizeof(T) == 8)
		{
			uint64 raw;
			memcpy(&raw, &value, sizeof(T));
			SetFgr64(cop0, index, raw);
		}
	}

	template <typename T>
	T Cop1::GetFgrBy(const Cop0& cop0, uint8 index) const
	{
		T value;
		if constexpr (sizeof(T) == 4)
		{
			const uint32 raw = GetFgr64(cop0, index);
			memcpy(&value, &raw, sizeof(T));
		}
		else if constexpr (sizeof(T) == 8)
		{
			const uint64 raw = GetFgr64(cop0, index);
			memcpy(&value, &raw, sizeof(T));
		}
		return value;
	}
}
