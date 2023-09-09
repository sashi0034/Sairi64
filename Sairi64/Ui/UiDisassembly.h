#pragma once

namespace Ui
{
	class UiDisassembly
	{
	public:
		void Update(std::string_view viewName, std::span<uint8> memory);

	private:
		Array<std::string> m_disassembledBuffer{};
		int m_baseAddr{};
	};
}
