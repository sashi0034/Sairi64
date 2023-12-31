﻿#include "stdafx.h"
#include "DuiManager.h"

#include "DuiDisassembly.h"
#include "DuiMemoryViewer.h"
#include "DuiTmemViewer.h"
#include "DuiUtil.h"
#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64Frame.h"
#include "N64/N64System.h"

class Dui::DuiManager::Impl
{
public:
	DuiExportData& ExportData() { return m_exportData; }

	void CheckWindowSize()
	{
		const auto currentWindowSize = Window::GetState().frameBufferSize;
		if (m_windowSize != currentWindowSize)
		{
			Scene::Resize(currentWindowSize);
			m_windowSize = currentWindowSize;
		}

		// const float fontScale = std::max(static_cast<float>(Scene::Size().x) / Window::GetState().virtualSize.x,
		// 						 static_cast<float>(Scene::Size().y) / Window::GetState().virtualSize.y);
		// constexpr float fontShrink = 0.75f;
		// ImGuiIO& io = ImGui::GetIO();
		// io.FontGlobalScale = fontScale * fontShrink;
	}

	void DefineDocking() const
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("Docking Fullscreen", nullptr,
		             ImGuiWindowFlags_NoTitleBar |
		             ImGuiWindowFlags_NoResize |
		             ImGuiWindowFlags_NoMove |
		             ImGuiWindowFlags_NoCollapse |
		             ImGuiWindowFlags_NoBringToFrontOnFocus |
		             ImGuiWindowFlags_NoNavFocus);

		ImGui::DockSpace(ImGui::GetID("Main docking"), ImVec2(0.0f, 0.0f),
		                 m_fullscreen ? ImGuiDockNodeFlags_None : ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}

	void UpdateDisplay(N64::N64System& n64)
	{
		ImGui::Begin("Main display");

		auto&& display = n64.GetRdp().GetDisplayTexture();
		if (m_mainDisplay.GetTexture() != display)
		{
			m_mainDisplay = ImS3dTexture(display);
		}
		constexpr float displayScale = 2.0f;
		const Float2 displaySize = display.size() * displayScale;
		const auto regionSize = ImGui::GetContentRegionAvail();
		if (const auto id = m_mainDisplay.GetId())
		{
			if (displaySize.x / regionSize.x > displaySize.y / regionSize.y)
			{
				// 幅で合わせる
				ImGui::Image(id.value(),
				             ImVec2{regionSize.x, displaySize.y * (regionSize.x / displaySize.x)});
			}
			else
			{
				// 高さで合わせる
				ImGui::Image(id.value(),
				             ImVec2{displaySize.x * (regionSize.y / displaySize.y), regionSize.y});
			}
		}

		ImGui::End();
	}

	void UpdateGeneral(N64::N64System& n64System, N64::N64Frame& n64Frame, const N64::N64Config& n64Config)
	{
		ImGui::Begin("General Status");
		using ss = std::stringstream;
		ImGui::Text((ss{} << "FPS: " << fmt::format("{:.2f}", n64Frame.Info().frameRate)).str().c_str());
		ImGui::Text((ss{} << "Frame count: " << n64Frame.Info().frameCount).str().c_str());
		ImGui::Text(fmt::format("Resolution: {}x{}",
		                        m_mainDisplay.GetTexture().size().x,
		                        m_mainDisplay.GetTexture().size().y).c_str());
		ImGui::Text((fmt::format("CPU PC: \n\t{:016X}\n\t{:016X}\n\t{:016X}",
		                         n64System.GetCpu().GetPc().Prev(),
		                         n64System.GetCpu().GetPc().Curr(),
		                         n64System.GetCpu().GetPc().Next()).c_str()));
		ImGui::Text((fmt::format("RSP PC: {:04X}, {:04X}, {:04X}",
		                         n64System.GetRsp().GetPc().Prev(),
		                         n64System.GetRsp().GetPc().Curr(),
		                         n64System.GetRsp().GetPc().Next()).c_str()));

		// システム
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

		// 音量調整
		if (m_globalVolume == -1 ||
			ImGui::SliderFloat("Global volume", &m_globalVolume, 0.0f, 1.0f))
		{
			if (m_globalVolume == -1) m_globalVolume = n64Config.audio.volume;
			n64System.GetAI().SetGlobalVolume(m_globalVolume);
		}

		ImGui::Checkbox("Power saving", &m_exportData.powerSaving);

		ImGui::End();
	}

	void UpdateContents(const N64::N64System& n64)
	{
		m_rdramViewer.Update("RDRAM Viewer", n64.GetMemory().Rdram());
		m_dmemViewer.Update("DMEM Viewer", n64.GetRsp().Dmem());
		m_imemViewer.Update("IMEM Viewer", n64.GetRsp().Imem());

		m_cpuDisassembly.Update("RDRAM Disassembly", n64.GetMemory().Rdram());
		m_rspDisassembly.Update("IMEM Disassembly", n64.GetRsp().Imem());

		m_tmemViewer.Update("TMEM Viewer", n64.GetRdp());
	}

private:
	DuiExportData m_exportData{
		.powerSaving = true
	};

	bool m_fullscreen = true;
	Size m_windowSize{};

	float m_globalVolume = -1;

	ImS3dTexture m_mainDisplay{Texture()};
	DuiMemoryViewer m_rdramViewer{};
	DuiMemoryViewer m_dmemViewer{};
	DuiMemoryViewer m_imemViewer{};
	DuiDisassembly m_cpuDisassembly{};
	DuiDisassembly m_rspDisassembly{};
	DuiTmemViewer m_tmemViewer{};
};

namespace Dui
{
	DuiManager::DuiManager() :
		m_impl(std::make_unique<Impl>())
	{
	}

	void DuiManager::Update(N64::N64System& n64System, N64::N64Frame& n64Frame, const N64::N64Config& n64Config)
	{
		m_impl->CheckWindowSize();

		m_impl->DefineDocking();

		// デモ
		// ImGui::ShowDemoWindow();

		// ゲーム画面
		m_impl->UpdateDisplay(n64System);

		// 一般
		m_impl->UpdateGeneral(n64System, n64Frame, n64Config);

		// 各要素
		m_impl->UpdateContents(n64System);
	}

	const DuiExportData& DuiManager::ExportData() const
	{
		return m_impl->ExportData();
	}

	DuiManager::ImplPtr::~ImplPtr() = default;
}
