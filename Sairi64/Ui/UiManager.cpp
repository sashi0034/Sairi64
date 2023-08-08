#include "stdafx.h"
#include "UiManager.h"

#include "Includes/imgui/imgui.h"

namespace Ui
{
	void UiManager::Update()
	{
		ImGui::ShowDemoWindow();

		// 固定ウィンドウ
		ImGui::SetNextWindowPos(ImVec2(100, 100));
		ImGui::SetNextWindowSize(ImVec2(200, 200));

		ImGui::Begin("Fixed test", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::Text("This window cannot be moved!");
		ImGui::End();
	}
}
