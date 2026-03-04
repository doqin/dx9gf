#include "SubScene.h"
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <utility>

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	auto app = DX9GF::Application::GetInstance();
	auto graphicsDevice = game->GetGraphicsDevice();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	square = std::make_shared<GO::Rectangle>(game->GetGraphicsDevice(), transformManager, 100, 100, app->GetScreenWidth() / 2 - 50, app->GetScreenHeight() / 2 - 50);
	square->Init();
	circle = std::make_shared<GO::Ellipse>(game->GetGraphicsDevice(), transformManager, 100, 100, 0, 0);
	circle->Init();
	transformManager->RebuildHierarchy();
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->GoToPrevious();
		return; // return otherwise we get a use after free situation
	}
	auto& js = game->GetJobSystem();
	js.Dispatch({
		.function = [deltaTime](void* data) {
			static_cast<GO::Rectangle*>(data)->Update(deltaTime);
		},
		.data=static_cast<void*>(square.get())
	});
	js.Dispatch({
		.function = [deltaTime](void* data) {
			static_cast<GO::Ellipse*>(data)->Update(deltaTime);
		},
		.data = static_cast<void*>(circle.get())
	});
	js.Wait();
	transformManager->UpdateAll(js);
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
			0xFFFF0000);
		square->Draw(deltaTime);
		circle->Draw(deltaTime);
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}
