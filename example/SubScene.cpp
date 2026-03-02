#include "SubScene.h"
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <utility>

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	auto app = DX9GF::Application::GetInstance();
	auto graphicsDevice = game->GetGraphicsDevice();
	square = std::make_shared<GO::Rectangle>(game->GetGraphicsDevice(), app->GetScreenWidth() / 2, app->GetScreenHeight() / 2, 100, 100);
	square->Init();
	circle = std::make_shared<GO::Ellipse>(game->GetGraphicsDevice(), 0, 0, 100, 100);
	circle->Init();
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->PopScene();
		return; // return otherwise we get a use after free situation
	}
	square->Update(deltaTime);
	circle->Update(deltaTime);
}

void SubScene::Draw(unsigned long long deltaTime)
{
	auto dev = game->GetGraphicsDevice();
	dev->Clear();
	if (SUCCEEDED(dev->BeginDraw())) {
		auto app = DX9GF::Application::GetInstance();
		auto width = app->GetScreenWidth();
		auto height = app->GetScreenHeight();
		DX9GF::Debug::DrawGrid(
			dev,
			0,
			0,
			width,
			height,
			32,
			32,
			0xFFFF0000
		);
		square->Draw(deltaTime);
		circle->Draw(deltaTime);
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}
