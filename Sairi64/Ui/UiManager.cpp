#include "stdafx.h"
#include "UiManager.h"

#include "Includes/imgui/imgui.h"

namespace Ui
{
	void UiManager::Update()
	{
		// ウィンドウサイズからフォントサイズ調整
		const float fontScale = std::max(static_cast<float>(Scene::Size().x) / Window::GetState().virtualSize.x,
		                                 static_cast<float>(Scene::Size().y) / Window::GetState().virtualSize.y);
		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = fontScale;

		// デモ
		ImGui::ShowDemoWindow();

		// 固定ウィンドウ
		// ImGui::SetNextWindowPos(ImVec2(0, 100));
		// ImGui::SetNextWindowSize(ImVec2(200, 200));
		//
		// ImGui::Begin("Fixed test", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		// ImGui::Text("This window cannot be moved!");
		// ImGui::End();
	}
}
