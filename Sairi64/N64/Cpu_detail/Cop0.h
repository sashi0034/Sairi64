#pragma once
#include "Utils/Util.h"

namespace N64::Cpu_detail
{
	using Utils::BitAccess;

	class ExceptionCode : public Utils::EnumValue<uint32>
	{
	public:
		constexpr explicit ExceptionCode(uint32 v): EnumValue(v) { return; }
	};

	// https://n64.readthedocs.io/#exception-codes
	namespace ExceptionKinds
	{
		constexpr ExceptionCode Interrupt{0};
		constexpr ExceptionCode TLBModification{1};
		constexpr ExceptionCode TLBMissLoad{2};
		constexpr ExceptionCode TLBMissStore{3};
		constexpr ExceptionCode AddressErrorLoad{4};
		constexpr ExceptionCode AddressErrorStore{5};
		constexpr ExceptionCode BusErrorInstructionFetch{6};
		constexpr ExceptionCode BusErrorLoadStore{7};
		constexpr ExceptionCode Syscall{8};
		constexpr ExceptionCode Breakpoint{9};
		constexpr ExceptionCode ReservedInstruction{10};
		constexpr ExceptionCode CoprocessorUnusable{11};
		constexpr ExceptionCode ArithmeticOverflow{12};
		constexpr ExceptionCode Trap{13};
		constexpr ExceptionCode FloatingPoint{15};
		constexpr ExceptionCode Watch{23};
	}

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
		ParityError = 26,
		CacheError = 27,
		TagLo = 28,
		TagHi = 29,
		ErrorEPC = 30,
		Unused_31 = 31,
	};

	// https://github.com/Dillonb/n64/blob/6502f7d2f163c3f14da5bff8cd6d5ccc47143156/src/cpu/r4300i.h#L276C2-L276C2
	class Cop0Status32
	{
	public:
		Cop0Status32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

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

		// [0, 15]
		auto De() { return BitAccess<16>(m_raw); } // 1
		auto Ce() { return BitAccess<17>(m_raw); } // 1
		auto Ch() { return BitAccess<18>(m_raw); } // 1
		// [19, 19]
		auto Sr() { return BitAccess<20>(m_raw); } // 1
		auto Ts() { return BitAccess<21>(m_raw); } // 1
		auto Bev() { return BitAccess<22>(m_raw); } // 1
		// [23, 23]
		auto Its() { return BitAccess<24>(m_raw); } // 1
		// [25, 31]
	private:
		uint32 m_raw{};
	};

	class Cop0Cause32
	{
	public:
		Cop0Cause32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

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

	// https://n64.readthedocs.io/#:~:text=for%20what%20purposes.-,COP0%20Registers,-Register%20Number
	struct Cop0Reg
	{
		uint32 index;
		// (1) random
		uint32 entryLo0; // TODO: refine type?
		uint32 entryLo1; // TODO: refine type?
		uint32 context; // TODO: refine type?
		uint32 pageMask; // TODO: refine type?
		uint32 wired;
		// (7)
		uint32 badVAddr;
		uint32 count;
		uint64 entryHi; // 64bit TODO: refine type?
		uint32 compare;
		Cop0Status32 status;
		Cop0Cause32 cause;
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
		Cop0Reg& Reg() { return m_reg; }

		uint64 Read64(uint8 number) const;
		uint32 Read32(uint8 number) const;
		void Write64(uint8 number, uint64 value);
		void Write32(uint8 number, uint32 value);

	private:
		template <typename Wire> Wire readInternal(uint8 number) const;
		template <typename Wire> void writeInternal(uint8 number, Wire value);
		Cop0Reg m_reg{};
		bool m_llBit{};
	};
}
