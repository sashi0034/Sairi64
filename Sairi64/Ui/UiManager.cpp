#include "stdafx.h"
#include "UiManager.h"

#include "N64/N64System.h"

namespace Ui
{
	void UiManager::Update(N64::N64System& n64)
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

		ImGui::Begin("FPS (debug)");
		using ss = std::stringstream;
		ImGui::Text((ss{} << "FPS: " << Profiler::FPS()).str().c_str());
		ImGui::Text((ss{} << "FrameCount: " << Scene::FrameCount()).str().c_str());
		ImGui::End();

		ImGui::Begin("CPU Status (debug)");
		ImGui::Text(U"PC: {:016X}"_fmt(n64.GetCpu().GetPc().Curr()).narrow().c_str());
		ImGui::End();
	}
}
