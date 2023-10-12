#pragma once

namespace Dui
{
	class DuiDisassembly
	{
	public:
		void Update(std::string_view viewName, std::span<const uint8> memory);

	private:
		Array<std::string> m_disassembledBuffer{};
		int m_baseAddr{};
	};
}
