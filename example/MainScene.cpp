#include "MainScene.h"
#include <DX9GFUtils.h>
#include "DX9GFGraphicsDevice.h"
#include "DX9GFSceneManager.h"
#include "SubScene.h"
#include "resource.h"

void MainScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	map = std::make_shared<DX9GF::Map>(game->GetGraphicsDevice());
	map->Create("./Resources/example.tmx");
	fontArial = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 32);
	whiteTexture = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	whiteTexture->CreatePlainTexture(0xFFFFFFFF, 500, 500);
	dawgTexture = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	dawgTexture->LoadTexture(IDR_RCDATA1, 100, 100);
	fontSpriteArial = std::make_shared<DX9GF::FontSprite>(fontArial.get());
	fontSpriteArial->SetOrigin(16, 16);
	fontSpriteArial->SetPosition(64, -64);
	fontSpriteArial->SetText(L"Hello world!");
	player = std::make_shared<GO::Player>(transformManager);
	player->Init(game->GetGraphicsDevice(), &camera, &worldColliders);
	rect = std::make_shared<GO::Rectangle>(transformManager, 64, 128, 64, 128);
	rect->Init(game->GetGraphicsDevice(), &camera, &worldColliders);
	colorRec = std::make_shared<DX9GF::StaticSprite>(whiteTexture.get());
	textureRec = std::make_shared<DX9GF::StaticSprite>(dawgTexture.get());
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
	if (inputManager->MousePress(DX9GF::InputManager::MouseButton::Middle)) {
		auto [dX, dY] = inputManager->GetRelativeMousePos();
		auto cameraPos = camera.GetPosition();
		cameraPos.x -= dX;
		cameraPos.y -= dY;
		camera.SetPosition(cameraPos);
	}
	auto dScroll = inputManager->GetMouseScroll();
	camera.SetZoom(camera.GetZoom() + dScroll / static_cast<float>(1000));
	fontSpriteArial->Rotate(D3DXToRadian(0.1f * deltaTime));
	player->Update(deltaTime);
	rect->Update(deltaTime);
	camera.Update();
	transformManager->UpdateAll(game->GetJobSystem());
}

void MainScene::Dispose()
{
}

void MainScene::Draw(unsigned long long deltaTime)
{
	auto dev = game->GetGraphicsDevice();
	dev->Clear();

	if (SUCCEEDED(dev->BeginDraw())) {
		auto app = DX9GF::Application::GetInstance();
		auto width = app->GetScreenWidth();
		auto height = app->GetScreenHeight();
		map->Draw(camera);
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
		textureRec->Begin();
		textureRec->Draw(camera, deltaTime);
		textureRec->End();
		rect->Draw(deltaTime);
		player->Draw(deltaTime);
		fontSpriteArial->Begin();
		fontSpriteArial->Draw(camera, deltaTime);
		fontSpriteArial->End();
		dev->EndDraw();
	}

	dev->Present();
}