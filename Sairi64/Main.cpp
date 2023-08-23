#include "stdafx.h"

#define CATCH_CONFIG_RUNNER
#include <ThirdParty/Catch2/catch.hpp>

#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64Config.h"
#include "N64/N64Frame.h"
#include "N64/N64Singleton.h"
#include "N64/N64System.h"
#include "Ui/UiManager.h"
#include "Utils/Util.h"

void setupWindow()
{
	// ウィンドウ、シーン設定など
	Window::SetTitle(U"GBEmu");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	constexpr Size sceneSize = {1920, 1080};
	Scene::Resize(sceneSize.x, sceneSize.y);
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
}

N64::N64Config debugConfig{
	.rom = {U"asset\\rom\\mimi-6126231.z64"}
};

void Main()
{
	Addon::Register<DearImGuiAddon>(U"ImGui");

	// テスト実行
	const bool isPassedTests = Catch::Session().run() == 0;
	if (!isPassedTests)
	{
		Utils::WaitAnyKeyOnConsole();
	}

	// コンソール起動
	Console.open();

	const auto n64 = std::make_unique<N64::N64Singleton>();
	N64::N64System& n64System = n64->GetSystem();
	N64::N64Frame n64Frame{};

	Ui::UiManager uiManager{};

	n64Frame.Init(n64System, {debugConfig.rom.filePath});

	// N64コンソール実行
	// n64Frame.RunOnConsole(n64System);

	setupWindow();

	while (System::Update())
	{
		// N64更新制御
		n64Frame.ControlFrame(n64System);
		n64System.GetRdp().RenderReal({32, 32});

		uiManager.Update(n64System);
	}
}
