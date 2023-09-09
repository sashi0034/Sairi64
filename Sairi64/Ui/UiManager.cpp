#include "stdafx.h"
#include "UiManager.h"

#include "UiMemoryViewer.h"
#include "UiUtil.h"
#include "N64/N64Frame.h"
#include "N64/N64System.h"

class Ui::UiManager::Impl
{
public:
	void Update(N64::N64System& n64)
	{
		m_rdramViewer.ShowMemoryView("RDRAM Viewer", n64.GetMemory().Rdram());
		m_dmemViewer.ShowMemoryView("DMEM Viewer", n64.GetRsp().Dmem());
		m_imemViewer.ShowMemoryView("IMEM Viewer", n64.GetRsp().Imem());
	}

private:
	UiMemoryViewer m_rdramViewer{};
	UiMemoryViewer m_dmemViewer{};
	UiMemoryViewer m_imemViewer{};
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

		ImGui::Begin("General status");
		using ss = std::stringstream;
		ImGui::Text((ss{} << "FPS: " << fmt::format("{:.2f}", n64Frame.frameRate)).str().c_str());
		ImGui::Text((ss{} << "Frame count: " << n64Frame.frameCount).str().c_str());
		ImGui::Text((fmt::format("CPU PC: {:016X}", n64System.GetCpu().GetPc().Curr()).c_str()));
		ImGui::Text((fmt::format("RSP PC: {:04X}", n64System.GetRsp().GetPc().Curr()).c_str()));
		ImGui::End();

		m_impl->Update(n64System);
	}

	UiManager::ImplPtr::~ImplPtr()
	{
	}
}
