﻿#include "stdafx.h"

#define CATCH_CONFIG_RUNNER
#include <ThirdParty/Catch2/catch.hpp>

#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64Config.h"
#include "N64/N64Frame.h"
#include "N64/N64Singleton.h"
#include "N64/N64System.h"
#include "Dui/DuiManager.h"
#include "Utils/Util.h"

void setupWindow()
{
	// ウィンドウ、シーン設定など
	Window::SetTitle(U"Sairi64");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	constexpr Size sceneSize = {1920, 1080};
	Scene::Resize(sceneSize.x, sceneSize.y);
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{U"#343541"});
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
}

void processDebugUi()
{
	Addon::Register<DearImGuiAddon>(U"ImGui");

#if not N64_RELEASE_ENABLE
	// テスト実行
	const bool isPassedTests = Catch::Session().run() == 0;
	if (!isPassedTests)
	{
		Utils::WaitAnyKeyOnConsole();
	}
#endif

	// コンソール起動
	Console.open();

	const auto n64 = std::make_unique<N64::N64Singleton>();
	N64::N64System& n64System = n64->GetSystem();
	N64::N64Frame n64Frame{};
	const N64::N64Config& n64Config = N64::N64Config::LoadToml(U"config.toml", U"asset/config_example.toml");

	Dui::DuiManager uiManager{};

	n64Frame.Init(n64System, n64Config);

	setupWindow();

	while (System::Update())
	{
		// N64更新制御
		n64Frame.ControlFrame(n64System, n64Config);
		// n64System.GetRdp().RenderReal({
		// 	.startPoint = {32, 32}, .scale = 2.0
		// });

		// フォーカスが当たっていないときはウィンドウを停止
		while (uiManager.ExportData().powerSaving && Window::GetState().focused == false)
		{
			n64Frame.HaltTasks();
			System::Sleep(1000 / Profiler::FPS());
		}

		uiManager.Update(n64System, n64Frame, n64Config);
	}

	n64Frame.HaltTasks();
}

void Main()
{
	processDebugUi();
}
