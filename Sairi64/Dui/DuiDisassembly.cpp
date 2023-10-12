#include "stdafx.h"
#include "DuiDisassembly.h"

#include "DuiUtil.h"
#include "N64/Instruction.h"
#include "N64/Debugger/DebugDisassembler.h"
#include "Utils/Util.h"

namespace Dui
{
	static constexpr int pageInstructionLength = 128;
	constexpr int showPageSize = pageInstructionLength * 4;

	void disassemblePage(Array<std::string>& buffer, std::span<const uint8> memory, int baseAddr)
	{
		if (buffer.size() < pageInstructionLength)
		{
			buffer.resize(pageInstructionLength);
		}
		for (int i = 0; i < pageInstructionLength; ++i)
		{
			const N64::Instruction instruction = Utils::ReadBytes<uint32>(memory, baseAddr + i * 4);
			auto disassembled = N64::Debugger::DebugDisassemble(instruction);
			buffer[i] = Unicode::Narrow(disassembled);
		}
	}

	void DuiDisassembly::Update(std::string_view viewName, std::span<const uint8> memory)
	{
		if (!ImGui::Begin(viewName.data()))
		{
			ImGui::End();
			return;
		}

		bool shouldRefresh = false;
		if (ImGui::Button("Refresh"))
		{
			shouldRefresh = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clipboard"))
		{
			int address = m_baseAddr - 4;
			Clipboard::SetText(m_disassembledBuffer
			                   .map([&](const std::string& str)
			                   {
				                   address += 4;
				                   return U"{:08X}: {}"_fmt(address, Unicode::Widen(str));
			                   })
			                   .join(U"\n"));
		}
		if (ImGui::Button("Back"))
		{
			m_baseAddr -= showPageSize;
			shouldRefresh = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Next"))
		{
			m_baseAddr += showPageSize;
			shouldRefresh = true;
		}

		char baseAddressStr[10];
		sprintf_s(baseAddressStr, "%08zx", m_baseAddr);

		// 16進数での入力を受け付けるテキストボックスを作成
		ImGui::PushItemWidth(Size_120);
		if (ImGui::InputText(
			"Base address",
			baseAddressStr,
			sizeof(baseAddressStr),
			ImGuiInputTextFlags_CharsHexadecimal))
		{
			sscanf_s(baseAddressStr, "%zx", &m_baseAddr);
			shouldRefresh = true;
		}

		if (shouldRefresh)
		{
			m_baseAddr = std::clamp(m_baseAddr, 0, static_cast<int>(memory.size()) - showPageSize);
			disassemblePage(m_disassembledBuffer, memory, m_baseAddr);
		}

		ImGui::BeginChild("View", ImVec2(0, 0), true);
		for (int i = 0; i < m_disassembledBuffer.size(); ++i)
		{
			// アドレス表示
			ImGui::TextColored(ImColorPurple, "%08zx:", m_baseAddr + i * 4);
			ImGui::SameLine();
			ImGui::TextColored(ImColorOrange, m_disassembledBuffer[i].data());
		}
		ImGui::EndChild();
		ImGui::End();
	}
}
