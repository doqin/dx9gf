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
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"StatusPlz", 16);
	saveManager = std::make_shared<DX9GF::SaveManager>();
	saveManager->Register(player.get());

	if (shouldLoadSave) {
		saveManager->Load("savegame.json");
	}

	savePoint = std::make_shared<SavePoint>(transformManager, 200.0f, 150.0f);
	savePoint->Init(game->GetGraphicsDevice(), &camera, player, saveManager, font);
	savePoint->SetVisible(true);
	transformManager->RebuildHierarchy();
}

void Demo::TutorialWorldScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
	if (inpMan->KeyPress(DIK_ESCAPE)) {
		auto sceMan = game->GetSceneManager();
		sceMan->PopScene();   
		sceMan->GoToPrevious();
		return;
	}
	bool isGamePaused = false;
	if (savePoint) {
		savePoint->Update(deltaTime);

		if (savePoint->IsMenuOpen()) {
			isGamePaused = true;
			inpMan->ReadMouse(deltaTime);
			inpMan->ReadKeyboard(deltaTime);
		}
	}
	if (!isGamePaused) {
		player->Update(deltaTime);
		transformManager->UpdateAll();
		camera.Update();
	}
}

void Demo::TutorialWorldScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear(0xFFFFFFFF);
	if (SUCCEEDED(gd->BeginDraw())) {
		map->Draw(camera);
		if (savePoint) {
			savePoint->Draw(camera, deltaTime);
		}
		player->Draw(deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}
