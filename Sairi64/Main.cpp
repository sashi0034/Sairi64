#include "stdafx.h"

#define CATCH_CONFIG_RUNNER
#include <ThirdParty/Catch2/catch.hpp>

#include "DearImGuiAddon/DearImGuiAddon.hpp"
#include "N64/N64Frame.h"
#include "N64/N64System.h"
#include "Ui/UiManager.h"
#include "Utils/Util.h"

void Main()
{
	Addon::Register<DearImGuiAddon>(U"ImGui");

	// テスト実行
	const bool isPassedTests = Catch::Session().run() == 0;
	if (!isPassedTests)
	{
		static_cast<void>(std::getchar());
	}

	// シーン設定など
	Console.open();

	Window::SetTitle(U"GBEmu");
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetResizeMode(ResizeMode::Keep);
	constexpr Size sceneSize = {1920, 1080};
	Scene::Resize(sceneSize.x, sceneSize.y);
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{0.3, 0.3, 0.3});
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	// ImGUI設定
	// ImGuiStyle& style = ImGui::GetStyle();
	// constexpr ImVec2 scaleFactor = ImVec2(1.5f, 1.5f);
	// style.ScaleAllSizes(scaleFactor.x);
	// ImGuiIO& io = ImGui::GetIO();
	// io.FontGlobalScale = scaleFactor.y;

	N64::N64System n64System{};
	N64::N64Frame n64Frame{};

	Ui::UiManager uiManager{};

	while (System::Update())
	{
		n64Frame.RunUntilAbort(n64System, 100);

		uiManager.Update();
	}
}
