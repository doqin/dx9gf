#include "MainScene.h"
#include <DX9GFUtils.h>
#include "DX9GFGraphicsDevice.h"
#include "DX9GFSceneManager.h"
#include "SubScene.h"

void MainScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	player = std::make_shared<GO::Player>(transformManager);
	player->Init(game->GetGraphicsDevice(), &camera, &worldColliders);
	rect = std::make_shared<GO::Rectangle>(transformManager, 64, 128, 64, 128);
	rect->Init(game->GetGraphicsDevice(), &camera, &worldColliders);
	// Creating colorRec
	colorRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	colorRec->CreatePlainTexture(0xFFFFFFFF, 500, 500);
	// Creating textureRec
	textureRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	textureRec->LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	auto app = DX9GF::Application::GetInstance();
	game->GetSceneManager()->PushScene(new SubScene(game, app->GetScreenWidth(), app->GetScreenHeight()));
	transformManager->RebuildHierarchy();
}

void MainScene::Update(unsigned long long deltaTime)
{	
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_ESCAPE)) PostMessage(game->GetHwnd(), WM_DESTROY, 0, 0);
	if (inputManager->KeyDown(DIK_F)) game->GetSceneManager()->GoToNext();
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
	player->Update(deltaTime);
	rect->Update(deltaTime);
	camera.Update();
	transformManager->UpdateAll(game->GetJobSystem());
}

void MainScene::Dispose()
{
	delete colorRec;
	delete textureRec;
}

void MainScene::Draw(unsigned long long deltaTime)
{
	auto dev = game->GetGraphicsDevice();
	dev->Clear();

	if (SUCCEEDED(dev->BeginDraw())) {
		auto app = DX9GF::Application::GetInstance();
		auto width = app->GetScreenWidth();
		auto height = app->GetScreenHeight();
		DX9GF::Debug::DrawGrid(
			dev,
			camera,
			0,
			0,
			width,
			height,
			32,
			32,
			0xFFFF0000);
		textureRec->Draw(camera);
		rect->Draw(deltaTime);
		player->Draw(deltaTime);
		dev->EndDraw();
	}

	dev->Present();
}