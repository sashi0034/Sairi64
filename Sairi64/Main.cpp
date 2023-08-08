#include "stdafx.h"

#define CATCH_CONFIG_RUNNER
#include <ThirdParty/Catch2/catch.hpp>

#include "Utils/Util.h"

void Main()
{
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

	while (System::Update())
	{
	}
}
