#include "stdafx.h"
#include "UiManager.h"

#include "N64/N64Frame.h"
#include "N64/N64System.h"

class Ui::UiManager::Impl
{
public:
};

namespace Ui
{
	UiManager::UiManager() :
		m_impl(std::make_unique<Impl>())
	{
	}

	void UiManager::Update(N64::N64System& n64System, const N64::N64FrameInfo& n64Frame)
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
		ImGui::Text((ss{} << "FrameCount: " << n64Frame.frameCount).str().c_str());
		ImGui::End();

		ImGui::Begin("CPU Status (debug)");
		ImGui::Text(U"PC: {:016X}"_fmt(n64System.GetCpu().GetPc().Curr()).narrow().c_str());
		ImGui::End();
	}

	UiManager::ImplPtr::~ImplPtr()
	{
	}
}
