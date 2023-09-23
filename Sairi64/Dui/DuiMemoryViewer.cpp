#include "stdafx.h"
#include "DuiMemoryViewer.h"

#include "DuiUtil.h"
#include "N64/N64System.h"

namespace Dui
{
	void DuiMemoryViewer::Update(std::string_view viewName, std::span<uint8> memory)
	{
		const uint8* data = memory.data();
		const uint32 dataSize = memory.size();

		if (!ImGui::Begin(viewName.data()))
		{
			ImGui::End();
			return;
		}

		// 表示する開始と終了のアドレスを持つスライダーを作成
		constexpr int32 showPageSize = 1024;

		if (ImGui::Button("Back"))
		{
			m_baseAddr -= showPageSize;
		}
		ImGui::SameLine();
		if (ImGui::Button("Next"))
		{
			m_baseAddr += showPageSize;
		}

		// ベースアドレスの16進数表示用の一時変数を定義
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
		}

		ImGui::VSliderInt(
			"##v",
			ImVec2(Size_20, ImGui::GetContentRegionAvail().y),
			&m_baseAddr,
			dataSize - showPageSize,
			0,
			"");
		m_baseAddr = std::clamp(m_baseAddr, 0, static_cast<int32>(dataSize - showPageSize));

		ImGui::SameLine();
		ImGui::BeginChild("View", ImVec2(0, 0), true);
		for (size_t addr = m_baseAddr; addr < m_baseAddr + showPageSize; addr += 16)
		{
			// アドレス表示
			ImGui::TextColored(ImColorPurple, "%08zx:", addr);

			// 16進数データ表示
			for (int i = 0; i < 16; i++)
			{
				ImGui::SameLine();
				if (i % 4 == 0)
				{
					ImGui::Text("");
					ImGui::SameLine();
				}
				if (addr + i < dataSize)
				{
					ImGui::TextColored(ImColorGreen, "%02x", data[addr + i]);
				}
				else
				{
					ImGui::Text("  ");
				}
			}
		}

		ImGui::EndChild();
		ImGui::End();
	}
}
