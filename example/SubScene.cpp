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
	whiteSquare->SetPosition(app->GetScreenWidth() / 2 - 50, app->GetScreenHeight() / 2 - 50);
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->PopScene();
		return;
	}
	if (isDragging && inputManager->MouseUp(DX9GF::InputManager::MouseButton::Left)) {
		isDragging = false;
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
	if (dev->BeginDraw()) {
		whiteSquare->Draw();
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
	delete whiteSquare;
}
