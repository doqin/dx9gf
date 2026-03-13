#include "MainScene.h"
#include <DX9GFUtils.h>
#include "DX9GFGraphicsDevice.h"
#include "DX9GFSceneManager.h"
#include "SubScene.h"
#include "resource.h"
#include "taskflow/taskflow.hpp"
#include "taskflow/algorithm/for_each.hpp"

void MainScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	audioManager = DX9GF::AudioManager::GetInstance();
	audioManager->Load("ALOVU", IDR_WAVE_ALOVU);
	transformManager = std::make_shared<DX9GF::TransformManager>();
	colliderManager = std::make_shared<DX9GF::ColliderManager>();
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
	players.push_back(std::make_shared<GO::Player>(transformManager));
	for (auto& player : players) {
		player->Init(game->GetGraphicsDevice(), &commandBuffer, &camera, colliderManager);
	}
	rect = std::make_shared<GO::Rectangle>(transformManager, 64, 128, 64, 128);
	rect->Init(game->GetGraphicsDevice(), &camera, colliderManager);
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
	if (inputManager->KeyDown(DIK_C)) {
		players.push_back(std::make_shared<GO::Player>(transformManager));
		players.back()->Init(game->GetGraphicsDevice(), &commandBuffer, &camera, colliderManager);
		transformManager->RebuildHierarchy();
	}
	float cameraXDir = 0;
	float cameraYDir = 0;
	const float cameraVelocity = 200;
	if (inputManager->KeyPress(DIK_LEFT)) cameraXDir -= 1;
	if (inputManager->KeyPress(DIK_RIGHT)) cameraXDir += 1;
	if (inputManager->KeyPress(DIK_UP)) cameraYDir -= 1;
	if (inputManager->KeyPress(DIK_DOWN)) cameraYDir += 1;
	if (inputManager->KeyDown(DIK_SPACE)) audioManager->Play("ALOVU");
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
	// Using task flow because I can't create a parallel programming system better than them (T_T)
	tf::Executor executor;
	tf::Taskflow taskflow;
	taskflow.for_each(players.begin(), players.end(), [deltaTime](std::shared_ptr<GO::Player> player) {
		player->Update(deltaTime);
	});
	executor.run(taskflow).wait();
	rect->Update(deltaTime);
	camera.Update();
	commandBuffer.Update(deltaTime);
	for (auto& player : players) {
		if (player && player->GetState() == DX9GF::IGameObject::State::Destroyed) {
			auto it = std::find(players.begin(), players.end(), player);
			players.erase(it);
		}
	}
	transformManager->UpdateAll();
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
		for (auto& player : players) player->Draw(deltaTime);
		fontSpriteArial->Begin();
		fontSpriteArial->Draw(camera, deltaTime);
		fontSpriteArial->End();
		dev->EndDraw();
	}

	dev->Present();
}