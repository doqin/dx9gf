#include "SubScene.h"
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <utility>

bool SubScene::IsWithinBoundCircle(float srcX, float srcY, float centerX, float centerY, float radius)
{
	return pow(centerX - srcX, 2) + pow(centerY - srcY, 2) < pow(radius, 2);
}

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	auto app = DX9GF::Application::GetInstance();
	auto graphicsDevice = game->GetGraphicsDevice();
	square = std::make_shared<GO::Rectangle>(game->GetGraphicsDevice(), app->GetScreenWidth() / 2, app->GetScreenHeight() / 2, 100, 100);
	square->Init();
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
	if (isDraggingCircle 
		&& inputManager->MouseUp(DX9GF::InputManager::MouseButton::Left
	)) {
		isDraggingCircle = false;
	}
	if (!isDraggingCircle
		&& inputManager->MouseDown(DX9GF::InputManager::MouseButton::Left)
		&& IsWithinBoundCircle(
			inputManager->GetAbsoluteMouseX(),
			inputManager->GetAbsoluteMouseY(),
			circleX,
			circleY,
			50
	)) {
		isDraggingCircle = true;
	}
	if (isDraggingCircle) {
		circleX += inputManager->GetRelativeMouseX();
		circleY += inputManager->GetRelativeMouseY();
	}
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
		game->GetGraphicsDevice()->DrawCircle(circleX, circleY, 50, 0xFFFFFF00, isDraggingCircle);
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}
