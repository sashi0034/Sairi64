#include "stdafx.h"
#include "Controller.h"

namespace N64::Joybus
{
	static const auto keyboardModifier = KeyShift | KeyControl | KeyAlt;

	Controller::Controller(int channel) :
		m_channel(channel)
	{
		using enum ControllerButtonKind;

		inputOf(DRight) = KeyRight;
		inputOf(DLeft) = KeyLeft;
		inputOf(DDown) = KeyDown;
		inputOf(DUp) = KeyUp;
		inputOf(Start) = KeyEnter;
		inputOf(Z) = KeyPeriod;
		inputOf(B) = KeySlash;
		inputOf(A) = KeyUnderscore_JIS;
		inputOf(CRight) = KeyNum6 | KeyEnd;
		inputOf(CLeft) = KeyNum4 | KeyHome;
		inputOf(CDown) = KeyNum2 | KeyPageDown;
		inputOf(CUp) = KeyNum8 | KeyPageUp;
		inputOf(R) = KeyX;
		inputOf(L) = KeyZ;
		inputOf(Zero) = {};
		inputOf(JoyReset) = {};
	}

	ControllerState Controller::ReadState() const
	{
		// 修飾キーが押されている間は入力無効にする
		if (keyboardModifier.pressed()) return {};

		using enum ControllerButtonKind;

		ControllerState state{};
		state.byte1.DRight().Set(inputOf(DRight).pressed());
		state.byte1.DLeft().Set(inputOf(DLeft).pressed());
		state.byte1.DDown().Set(inputOf(DDown).pressed());
		state.byte1.DUp().Set(inputOf(DUp).pressed());
		state.byte1.Start().Set(inputOf(Start).pressed());
		state.byte1.Z().Set(inputOf(Z).pressed());
		state.byte1.B().Set(inputOf(B).pressed());
		state.byte1.A().Set(inputOf(A).pressed());

		state.byte2.CRight().Set(inputOf(CRight).pressed());
		state.byte2.CLeft().Set(inputOf(CLeft).pressed());
		state.byte2.CDown().Set(inputOf(CDown).pressed());
		state.byte2.CUp().Set(inputOf(CUp).pressed());
		state.byte2.R().Set(inputOf(R).pressed());
		state.byte2.L().Set(inputOf(L).pressed());
		state.byte2.Zero().Set(false);
		state.byte2.JoyReset().Set(inputOf(L).pressed() && inputOf(R).pressed() && inputOf(Start).pressed());

		// ジョイスティック入力
		constexpr sint8 keyboardAxisSensitivity = 100;
		if (KeyA.pressed()) state.joyX = -keyboardAxisSensitivity;
		if (KeyD.pressed()) state.joyX = keyboardAxisSensitivity;
		if (KeyW.pressed()) state.joyY = -keyboardAxisSensitivity;
		if (KeyS.pressed()) state.joyY = keyboardAxisSensitivity;

		return state;
	}

	InputGroup& Controller::inputOf(ControllerButtonKind button)
	{
		return m_inputGroups[static_cast<int>(button)];
	}

	const InputGroup& Controller::inputOf(ControllerButtonKind button) const
	{
		return m_inputGroups[static_cast<int>(button)];
	}
}
