#pragma once
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	using Utils::BitAccess;

	constexpr uint32 Cop0Regs_32 = 32;

	constexpr StringView Cop0RegName_Unused = U"Unused";

	enum class Cop0RegKind : uint8
	{
		Index = 0,
		Random = 1,
		EntryLo0 = 2,
		EntryLo1 = 3,
		Context = 4,
		PageMask = 5,
		Wired = 6,
		Unused_7 = 7,
		BadVAddr = 8,
		Count = 9,
		EntryHi = 10,
		Compare = 11,
		Status = 12,
		Cause = 13,
		EPC = 14,
		PRId = 15,
		Config = 16,
		LLAddr = 17,
		WatchLo = 18,
		WatchHi = 19,
		XContext = 20,
		Unused_21 = 21,
		Unused_22 = 22,
		Unused_23 = 23,
		Unused_24 = 24,
		Unused_25 = 25,
		ECC = 26,
		CacheErr = 27,
		TagLo = 28,
		TagHi = 29,
		ErrEPC = 30,
		Unused_31 = 31,
	};

	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/r4300i.h#L276C2-L276C2
	class Cop0Status
	{
	public:
		Cop0Status(uint32 raw = 0): m_raw(raw) { return; }

		auto Ie() { return BitAccess<0>(m_raw); } // 1
		auto Exl() { return BitAccess<1>(m_raw); } // 1
		auto Erl() { return BitAccess<2>(m_raw); } // 1
		auto Ksu() { return BitAccess<3, 4>(m_raw); } // 2
		auto Ux() { return BitAccess<5>(m_raw); } // 1
		auto Sx() { return BitAccess<6>(m_raw); } // 1
		auto Kx() { return BitAccess<7>(m_raw); } // 1
		auto Im() { return BitAccess<8, 15>(m_raw); } // 8
		auto Ds() { return BitAccess<16, 24>(m_raw); } // 9
		auto Re() { return BitAccess<25>(m_raw); } // 1
		auto Fr() { return BitAccess<26>(m_raw); } // 1
		auto Rp() { return BitAccess<27>(m_raw); } // 1
		auto Cu0() { return BitAccess<28>(m_raw); } // 1
		auto Cu1() { return BitAccess<29>(m_raw); } // 1
		auto Cu2() { return BitAccess<30>(m_raw); } // 1
		auto Cu3() { return BitAccess<31>(m_raw); } // 1

	private:
		uint32 m_raw{};
	};

	class Cop0Cause
	{
	public:
		Cop0Cause(uint32 raw = 0): m_raw(raw) { return; }

		auto InterruptPending() { return BitAccess<8, 15>(m_raw); }

		// [0, 1]
		auto ExceptionCode() { return BitAccess<2, 6>(m_raw); }
		// [7, 7]
		auto Ip0() { return BitAccess<8>(m_raw); }
		auto Ip1() { return BitAccess<9>(m_raw); }
		auto Ip2() { return BitAccess<10>(m_raw); }
		auto Ip3() { return BitAccess<11>(m_raw); }
		auto Ip4() { return BitAccess<12>(m_raw); }
		auto Ip5() { return BitAccess<13>(m_raw); }
		auto Ip6() { return BitAccess<14>(m_raw); }
		auto Ip7() { return BitAccess<15>(m_raw); }
		// [16, 27]
		auto CoprocessorError() { return BitAccess<28, 29>(m_raw); }
		// [30, 30]
		auto BranchDelay() { return BitAccess<31>(m_raw); }

	private:
		uint32 m_raw{};
	};

	struct Cop0Reg
	{
		uint32 index;
		uint32 random;
		uint32 entryLo0; // TODO: refine type?
		uint32 entryLo1; // TODO: refine type?
		uint32 context; // TODO: refine type?
		uint32 page_mask; // TODO: refine type?
		uint32 wired;
		// (7)
		uint32 badVAddr;
		uint32 count;
		uint64 entryHi; // 64bit TODO: refine type?
		uint32 compare;
		Cop0Status status; // TODO: refine type?
		Cop0Cause cause; // TODO: refine type?
		uint64 epc; // 64bit
		uint32 prId;
		uint32 config;
		uint32 llAddr;
		uint32 watchLo; // TODO: refine type?
		uint32 watchHi;
		uint64 xContext; // TODO: refine type?
		// (21)
		// (22)
		// (23)
		// (24)
		// (25)
		uint32 parityError;
		uint32 cacheError;
		uint32 tagLo;
		uint32 tagHi;
		uint64 errorEpc; // 64bit
		// (31)
	};

	class Cop0
	{
	public:
		Cop0();
		const Cop0Reg& Reg() const { return m_reg; }
		Cop0Reg& TouchReg() { return m_reg; }

		uint64 Read(uint8 number);

	private:
		Cop0Reg m_reg{};
		bool m_llBit{};
	};
}
