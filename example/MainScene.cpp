#include "MainScene.h"
#include <DX9GFUtils.h>
#include "DX9GFGraphicsDevice.h"
#include "DX9GFSceneManager.h"
#include "SubScene.h"

void MainScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	// Creating colorRec
	colorRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	colorRec->CreatePlainTexture(0xFFFFFFFF, 500, 500);
	// Creating textureRec
	textureRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	textureRec->LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	// Creating mario
	mario = new DX9GF::AnimatedSprite(game->GetGraphicsDevice());
	mario->LoadSpriteSheet(
		L".\\Resources\\spritesheet.png",
		DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3),
		156,
		7497
	);
}

void MainScene::Update(unsigned long long deltaTime)
{
	auto app = DX9GF::Application::GetInstance();
	inputManager->ReadKeyboard(deltaTime);
	if (inputManager->KeyDown(DIK_ESCAPE)) PostMessage(game->GetHwnd(), WM_DESTROY, 0, 0);
	float xDir = 0;
	float yDir = 0;
	const float velocity = 200;
	if (inputManager->KeyPress(DIK_A)) xDir -= 1;
	if (inputManager->KeyPress(DIK_D)) xDir += 1;
	if (inputManager->KeyPress(DIK_W)) yDir -= 1;
	if (inputManager->KeyPress(DIK_S)) yDir += 1;
	if (inputManager->KeyDown(DIK_F)) game->GetSceneManager()->PushScene(new SubScene(game, app->GetScreenWidth(), app->GetScreenHeight()));
	mario->Translate(xDir * velocity * deltaTime / 1000, yDir * velocity * deltaTime / 1000);
	
	float cameraXDir = 0;
	float cameraYDir = 0;
	const float cameraVelocity = 200;
	if (inputManager->KeyPress(DIK_LEFT)) cameraXDir -= 1;
	if (inputManager->KeyPress(DIK_RIGHT)) cameraXDir += 1;
	if (inputManager->KeyPress(DIK_UP)) cameraYDir -= 1;
	if (inputManager->KeyPress(DIK_DOWN)) cameraYDir += 1;
	if (cameraXDir != 0 || cameraYDir != 0) {
		auto cameraPos = camera.GetPosition();
		cameraPos.x += cameraXDir * cameraVelocity * deltaTime / 1000;
		cameraPos.y += cameraYDir * cameraVelocity * deltaTime / 1000;
		camera.SetPosition(cameraPos);
	}
	camera.Update();
}

void MainScene::Dispose()
{
	delete colorRec;
	delete textureRec;
	delete mario;
}

void MainScene::Draw(unsigned long long deltaTime)
{
	auto dev = game->GetGraphicsDevice();
	dev->Clear();

	if (dev->BeginDraw()) {
		auto app = DX9GF::Application::GetInstance();
		auto width = app->GetScreenWidth();
		auto height = app->GetScreenHeight();
		auto cameraOffsets = camera.GetPosition();
		DX9GF::Debug::DrawGrid(
			dev,
			-cameraOffsets.x,
			-cameraOffsets.y,
			width,
			height,
			32,
			32,
			0xFFFF0000);
		textureRec->Draw(camera);
		mario->Draw(camera);
		dev->EndDraw();
	}

	dev->Present();
}