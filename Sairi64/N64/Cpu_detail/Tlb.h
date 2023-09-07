#pragma once
#include "N64/Forward.h"

namespace N64::Cpu_detail
{
	using namespace Utils;

	class PageMask32
	{
	public:
		PageMask32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		// [0, 12] 13
		auto Mask() { return BitAccess<13, 24>(m_raw); } // 12
		// [25, 31] 7
	private:
		uint32 m_raw{};
	};

	class TlbIndex32
	{
	public:
		TlbIndex32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto I() { return BitAccess<0, 5>(m_raw); } // 6
		// [6, 30] 25
		auto P() { return BitAccess<31>(m_raw); } // 1
	private:
		uint32 m_raw{};
	};

	class EntryLo32
	{
	public:
		EntryLo32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto G() { return BitAccess<0>(m_raw); } // 1
		auto V() { return BitAccess<1>(m_raw); } // 1
		auto D() { return BitAccess<2>(m_raw); } // 1
		auto C() { return BitAccess<3, 5>(m_raw); } // 3
		auto Pfn() { return BitAccess<6, 25>(m_raw); } // 20
		// [26, 31] 6
	private:
		uint32 m_raw{};
	};

	class EntryHi64
	{
	public:
		EntryHi64(uint64 raw = 0): m_raw(raw) { return; }
		operator uint64() const { return m_raw; }

		auto AsId() { return BitAccess<0, 7>(m_raw); } // 8
		// [8, 12] 5
		auto Vpn2() { return BitAccess<13, 39>(m_raw); } // 27
		auto Fill() { return BitAccess<40, 61>(m_raw); } // 22
		auto R() { return BitAccess<62, 63>(m_raw); } // 2

	private:
		uint64 m_raw{};
	};

	struct TlbEntry
	{
		bool initialized;
		EntryLo32 entryLo0;
		EntryLo32 entryLo1;
		EntryHi64 entryHi;
		PageMask32 pageMask;
		bool global;
	};

	constexpr uint8 TlbSize_32 = 32;

	enum class TlbError : uint8
	{
		None,
		Miss,
		Invalid,
		Modification,
		DisallowedAddress
	};

	class Tlb
	{
	public:
		void WriteEntry(uint32 index);
		TlbError GetError() const { return m_error; }

	private:
		PageMask32 m_pageMask{};
		EntryHi64 m_entryHi{};
		EntryLo32 m_entryLo0{};
		EntryLo32 m_entryLo1{};
		TlbIndex32 m_index{};
		std::array<TlbEntry, TlbSize_32> m_tlb{};
		TlbError m_error{};
	};
}
