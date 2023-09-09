#pragma once

namespace Ui
{
	enum class DisassembleKind
	{
		Cpu,
		Rsp,
	};

	class UiDisassembly
	{
	public:
		void Update(std::string_view viewName, std::span<uint8> memory, DisassembleKind kind);

	private:
		Array<std::string> m_disassembledBuffer{};
		int m_baseAddr{};
	};
}
