#include "SubScene.h"
#include "DX9GFSceneManager.h"

bool SubScene::IsWithinBound(float srcX, float srcY, float dstX, float dstY, float dstW, float dstH)
{
	return srcX > dstX && srcX < dstX + dstW && srcY > dstY && srcY < dstY + dstH;
}

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	whiteSquare = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	whiteSquare->CreatePlainTexture(0xFFFFFFFF, 100, 100);
	auto app = DX9GF::Application::GetInstance();
	whiteSquare->SetPosition(app->GetScreenWidth() / 2.0 - 50, app->GetScreenHeight() / 2.0 - 50);
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->PopScene();
		return; // return otherwise we get a use after free situation
	}
	if (isDragging && inputManager->MouseUp(DX9GF::InputManager::MouseButton::Left)) {
		isDragging = false;
		whiteSquare->SetColor(0xFFFFFFFF);
	}
	if (!isDragging
		&& inputManager->MouseDown(DX9GF::InputManager::MouseButton::Left) 
		&& IsWithinBound(
			inputManager->GetAbsoluteMouseX(),
			inputManager->GetAbsoluteMouseY(),
			whiteSquare->GetPositionX(),
			whiteSquare->GetPositionY(),
			100,
			100)) {
		isDragging = true;
		whiteSquare->SetColor(0xFF00FF00);
	}
	
	if (isDragging) {
		whiteSquare->Translate(
			inputManager->GetRelativeMouseX(), 
			inputManager->GetRelativeMouseY()
		);
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
		whiteSquare->Draw();
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
	delete whiteSquare;
}
