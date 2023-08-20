#pragma once
#include "N64/Forward.h"
#include "N64/Interrupt.h"

namespace N64::Mmio
{
	namespace MiAddress
	{
		constexpr PAddr32 Mode_0x04300000{0x04300000};
		constexpr PAddr32 Version_0x04300004{0x04300004};
		constexpr PAddr32 Interrupt_0x04300008{0x04300008};
		constexpr PAddr32 InterruptMask_0x0430000C{0x0430000C};
	}

	using Utils::BitAccess;

	class MiInterrupt32
	{
	public:
		MiInterrupt32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto Sp() { return BitAccess<0>(m_raw); } // 1
		auto Si() { return BitAccess<1>(m_raw); } // 1
		auto Ai() { return BitAccess<2>(m_raw); } // 1
		auto Vi() { return BitAccess<3>(m_raw); } // 1
		auto Pi() { return BitAccess<4>(m_raw); } // 1
		auto Dp() { return BitAccess<5>(m_raw); } // 1
		// [6, 31]
	private:
		uint32 m_raw{};
	};

	class MiMode32
	{
	public:
		MiMode32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto InitLength() { return BitAccess<0, 6>(m_raw); }
		auto InitMode() { return BitAccess<7>(m_raw); }
		auto EbusTestMode() { return BitAccess<8>(m_raw); }
		auto RdramRegisterMode() { return BitAccess<9>(m_raw); }

	private:
		uint32 m_raw{};
	};

	class MiModeWrite32
	{
	public:
		MiModeWrite32(uint32 raw = 0): m_raw(raw) { return; }
		operator uint32() const { return m_raw; }

		auto InitLength() { return BitAccess<0, 6>(m_raw); }
		auto ClearInitMode() { return BitAccess<7>(m_raw); }
		auto SetInitMode() { return BitAccess<8>(m_raw); }
		auto ClearEbusTestMode() { return BitAccess<9>(m_raw); }
		auto SetEbusTestMode() { return BitAccess<10>(m_raw); }
		auto ClearDpInterrupt() { return BitAccess<11>(m_raw); }
		auto ClearRdramRegisterMode() { return BitAccess<12>(m_raw); }
		auto SetRdramRegisterMode() { return BitAccess<13>(m_raw); }

	private:
		uint32 m_raw{};
	};

	// https://n64brew.dev/wiki/MIPS_Interface
	// MIPS Interface
	class MI
	{
	public:
		uint32 Read32(PAddr32 paddr) const;
		void Write32(N64System& n64, PAddr32 paddr, uint32 value);

		template <Interruption interrupt, bool flag> void ChangeInterrupt();
		MiInterrupt32 GetInterrupt() const { return m_interrupt; }
		MiInterrupt32 GetInterruptMask() const { return m_interruptMask; }

	private:
		MiMode32 m_mode{};
		MiInterrupt32 m_interrupt{};
		MiInterrupt32 m_interruptMask{};

		void writeMode(N64System& n64, MiModeWrite32 write);
		void writeInterruptMask(N64System& n64, uint32 write);
	};

	template <Interruption interrupt, bool flag>
	void MI::ChangeInterrupt()
	{
		if constexpr (interrupt == Interruption::VI)
			m_interrupt.Vi().Set(flag);
		else if constexpr (interrupt == Interruption::SI)
			m_interrupt.Si().Set(flag);
		else if constexpr (interrupt == Interruption::PI)
			m_interrupt.Pi().Set(flag);
		else if constexpr (interrupt == Interruption::AI)
			m_interrupt.Ai().Set(flag);
		else if constexpr (interrupt == Interruption::DP)
			m_interrupt.Dp().Set(flag);
		else if constexpr (interrupt == Interruption::SP)
			m_interrupt.Sp().Set(flag);
		else
		{
			static_assert(InterruptionFalse<interrupt>);
		}
	}
}
