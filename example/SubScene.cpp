#include "SubScene.h"
#include "DX9GFSceneManager.h"

bool SubScene::IsWithinBoundRectangle(float srcX, float srcY, float dstX, float dstY, float dstW, float dstH)
{
	return srcX > dstX && srcX < dstX + dstW && srcY > dstY && srcY < dstY + dstH;
}

bool SubScene::IsWithinBoundCircle(float srcX, float srcY, float centerX, float centerY, float radius)
{
	return pow(centerX - srcX, 2) + pow(centerY - srcY, 2) < pow(radius, 2);
}

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	auto app = DX9GF::Application::GetInstance();
	squareX = app->GetScreenWidth() / 2.0 - 50;
	squareY = app->GetScreenHeight() / 2.0 - 50;
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->PopScene();
		return; // return otherwise we get a use after free situation
	}
	if (isDraggingSquare && inputManager->MouseUp(DX9GF::InputManager::MouseButton::Left)) {
		isDraggingSquare = false;
	}
	bool isWithinBound = IsWithinBoundRectangle(
		inputManager->GetAbsoluteMouseX(),
		inputManager->GetAbsoluteMouseY(),
		squareX,
		squareY,
		100,
		100);
	if (!isDraggingSquare
		&& inputManager->MouseDown(DX9GF::InputManager::MouseButton::Left) 
		&& isWithinBound) {
		isDraggingSquare = true;
	}
	
	if (isDraggingSquare) {
		squareX += inputManager->GetRelativeMouseX();
		squareY += inputManager->GetRelativeMouseY();
	}

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
		game->GetGraphicsDevice()->DrawRectangle(squareX, squareY, 100, 100, 0xFF0000FF, isDraggingSquare);
		game->GetGraphicsDevice()->DrawCircle(circleX, circleY, 50, 0xFFFFFF00, isDraggingCircle);
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}
