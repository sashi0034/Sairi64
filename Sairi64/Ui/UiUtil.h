#pragma once

namespace Ui
{
	constexpr ImVec4 HexToImVec4(uint32_t hex)
	{
		return ImVec4(
			((hex >> 24) & 0xFF) / 255.0f, // R
			((hex >> 16) & 0xFF) / 255.0f, // G
			((hex >> 8) & 0xFF) / 255.0f, // B
			(hex & 0xFF) / 255.0f // A
		);
	}

	constexpr ImVec4 ImColorGreen = HexToImVec4(0x60e010ff);
	constexpr ImVec4 ImColorPurple = HexToImVec4(0xff00dcff);
	constexpr ImVec4 ImColorOrange = HexToImVec4(0xff7e01ff);

	constexpr int Size_20 = 20;
	constexpr int Size_120 = 120;
}
