#include "stdafx.h"
#include "UiManager.h"

#include "UiDisassembly.h"
#include "UiMemoryViewer.h"
#include "UiUtil.h"
#include "N64/N64Frame.h"
#include "N64/N64System.h"

class Ui::UiManager::Impl
{
public:
	void Update(N64::N64System& n64)
	{
		m_rdramViewer.Update("RDRAM Viewer", n64.GetMemory().Rdram());
		m_dmemViewer.Update("DMEM Viewer", n64.GetRsp().Dmem());
		m_imemViewer.Update("IMEM Viewer", n64.GetRsp().Imem());

		m_cpuDisassembly.Update("RDRAM Disassembly", n64.GetMemory().Rdram(), DisassembleKind::Cpu);
		m_rspDisassembly.Update("IMEM Disassembly", n64.GetRsp().Imem(), DisassembleKind::Rsp);
	}

private:
	UiMemoryViewer m_rdramViewer{};
	UiMemoryViewer m_dmemViewer{};
	UiMemoryViewer m_imemViewer{};
	UiDisassembly m_cpuDisassembly{};
	UiDisassembly m_rspDisassembly{};
};

namespace Ui
{
	UiManager::UiManager() :
		m_impl(std::make_unique<Impl>())
	{
	}

	void UiManager::Update(N64::N64System& n64System, N64::N64Frame& n64Frame, const N64::N64Config& n64Config)
	{
		// ウィンドウサイズからフォントサイズ調整
		const float fontScale = std::max(static_cast<float>(Scene::Size().x) / Window::GetState().virtualSize.x,
		                                 static_cast<float>(Scene::Size().y) / Window::GetState().virtualSize.y);
		ImGuiIO& io = ImGui::GetIO();
		io.FontGlobalScale = fontScale;

		// デモ
		ImGui::ShowDemoWindow();

		ImGui::Begin("General Status");
		using ss = std::stringstream;
		ImGui::Text((ss{} << "FPS: " << fmt::format("{:.2f}", n64Frame.Info().frameRate)).str().c_str());
		ImGui::Text((ss{} << "Frame count: " << n64Frame.Info().frameCount).str().c_str());
		ImGui::Text((fmt::format("CPU PC: {:016X}", n64System.GetCpu().GetPc().Curr()).c_str()));
		ImGui::Text((fmt::format("RSP PC: {:04X}", n64System.GetRsp().GetPc().Curr()).c_str()));

		if (n64Frame.IsSuspended() == false)
		{
			if (ImGui::Button("Suspend system"))
			{
				n64Frame.SetSuspend(true);
			}
		}
		else
		{
			if (ImGui::Button("Resume system"))
			{
				n64Frame.SetSuspend(false);
			}
			ImGui::SameLine();
			if (ImGui::Button("Step system"))
			{
				n64Frame.StepSingleFrame(n64System, n64Config);
			}
		}
		ImGui::End();

		m_impl->Update(n64System);
	}

	UiManager::ImplPtr::~ImplPtr() = default;
}
