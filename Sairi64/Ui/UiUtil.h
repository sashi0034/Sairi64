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
}
