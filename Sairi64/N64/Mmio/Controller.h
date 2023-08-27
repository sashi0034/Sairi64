#pragma once
#include "Utils/Util.h"

namespace N64::Mmio
{
	using namespace Utils;

	class ControllerByte1
	{
	public:
		ControllerByte1(uint8 raw = 0): m_raw(raw) { return; }
		operator uint8() const { return m_raw; }

		auto DRight() { return BitAccess<0>(m_raw); } // 1
		auto DLeft() { return BitAccess<1>(m_raw); } // 1
		auto DDown() { return BitAccess<2>(m_raw); } // 1
		auto DUp() { return BitAccess<3>(m_raw); } // 1
		auto Start() { return BitAccess<4>(m_raw); } // 1
		auto Z() { return BitAccess<5>(m_raw); } // 1
		auto B() { return BitAccess<6>(m_raw); } // 1
		auto A() { return BitAccess<7>(m_raw); } // 1
	private:
		uint8 m_raw{};
	};

	class ControllerByte2
	{
	public:
		ControllerByte2(uint8 raw = 0): m_raw(raw) { return; }
		operator uint8() const { return m_raw; }

		auto CRight() { return BitAccess<0>(m_raw); } // 1
		auto CLeft() { return BitAccess<1>(m_raw); } // 1
		auto CDown() { return BitAccess<2>(m_raw); } // 1
		auto CUp() { return BitAccess<3>(m_raw); } // 1
		auto R() { return BitAccess<4>(m_raw); } // 1
		auto L() { return BitAccess<5>(m_raw); } // 1
		auto Zero() { return BitAccess<6>(m_raw); } // 1
		auto JoyReset() { return BitAccess<7>(m_raw); } // 1
	private:
		uint8 m_raw{};
	};

	enum class ControllerButtonKind
	{
		DRight = 0,
		DLeft,
		DDown,
		DUp,
		Start,
		Z,
		B,
		A,
		CRight,
		CLeft,
		CDown,
		CUp,
		R,
		L,
		Zero,
		JoyReset,
		Max
	};

	constexpr int ControllerButtonSize = static_cast<int>(ControllerButtonKind::Max);

	struct ControllerState
	{
		ControllerByte1 byte1{};
		ControllerByte2 byte2{};
		sint8 joyX{};
		sint8 joyY{};
	};

	class Controller
	{
	public:
		Controller();
		ControllerState ReadState() const;

	private:
		std::array<InputGroup, ControllerButtonSize> m_inputGroups{};
		InputGroup& inputOf(ControllerButtonKind button);
		const InputGroup& inputOf(ControllerButtonKind button) const;
	};
}
