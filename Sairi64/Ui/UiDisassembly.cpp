#include "stdafx.h"
#include "UiDisassembly.h"

#include "UiUtil.h"
#include "N64/Instruction.h"
#include "N64/Debugger/CpuDisassembler.h"
#include "N64/Debugger/RspDisassembler.h"
#include "Utils/Util.h"

namespace Ui
{
	static constexpr int pageInstructionLength = 128;
	constexpr int showPageSize = pageInstructionLength * 4;

	void disassemblePage(Array<std::string>& buffer, std::span<uint8> memory, DisassembleKind kind, int baseAddr)
	{
		if (buffer.size() < pageInstructionLength)
		{
			buffer.resize(pageInstructionLength);
		}
		for (int i = 0; i < pageInstructionLength; ++i)
		{
			const N64::Instruction instruction = Utils::ReadBytes<uint32>(memory, baseAddr + i * 4);
			auto disassembled = kind == DisassembleKind::Cpu
			                    ? N64::Debugger::CpuDisassembler::Disassemble(instruction)
			                    : N64::Debugger::RspDisassembler::Disassemble(instruction);
			buffer[i] = Unicode::Narrow(disassembled);
		}
	}

	void UiDisassembly::Update(std::string_view viewName, std::span<uint8> memory, DisassembleKind kind)
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
			disassemblePage(m_disassembledBuffer, memory, kind, m_baseAddr);
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
