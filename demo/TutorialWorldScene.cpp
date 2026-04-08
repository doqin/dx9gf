#include "pch.h"
#include "TutorialWorldScene.h"

void Demo::TutorialWorldScene::Init()
{
	camera.SetZoom(2.0f);
	transformManager = std::make_shared<DX9GF::TransformManager>();
	colliderManager = std::make_shared<DX9GF::ColliderManager>();
	player = std::make_shared<Player>(transformManager);
	player->Init(game->GetGraphicsDevice(), colliderManager.get(), &camera);
	map = std::make_shared<DX9GF::Map>(game->GetGraphicsDevice());
	map->Create(transformManager, colliderManager, "./tutorial.tmx");
	transformManager->RebuildHierarchy();
}

void Demo::TutorialWorldScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
	player->Update(deltaTime);
	transformManager->UpdateAll();
	camera.Update();
}

void Demo::TutorialWorldScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear(0xFFFFFFFF);
	if (SUCCEEDED(gd->BeginDraw())) {
		map->Draw(camera);
		player->Draw(deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}
