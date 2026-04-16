#include "pch.h"
#include "TutorialWorldScene.h"
void Demo::TutorialWorldScene::Init()
{
	camera.SetZoom(2.0f);
	transformManager = std::make_shared<DX9GF::TransformManager>();
	colliderManager = std::make_shared<DX9GF::ColliderManager>();
	player = std::make_shared<Player>(transformManager, 248, 184);
	camera.SetPosition(248, 184);
	player->Init(game->GetGraphicsDevice(), colliderManager.get(), &camera);
	map = std::make_shared<DX9GF::Map>(game->GetGraphicsDevice());
	map->Create(transformManager, colliderManager, "./tutorial.tmx");
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"StatusPlz", 16);

	savePoint = std::make_shared<SavePoint>(transformManager, 200.0f, 150.0f);
	savePoint->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font);
	savePoint->SetVisible(true);
	transformManager->RebuildHierarchy();
}

void Demo::TutorialWorldScene::Update(unsigned long long deltaTime)
{
	auto [currentWidth, currentHeight] = camera.GetScreenResolution();
	auto [lastWidth, lastHeight] = uiCamera.GetScreenResolution();
	if (currentWidth != lastWidth || currentHeight != lastHeight) {
		uiCamera.SetScreenResolution(currentWidth, currentHeight);
	}
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
		DX9GF::InputManager::GetInstance()->DrawCursor(&this->uiCamera, deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}

std::string Demo::TutorialWorldScene::GetSaveID() const
{
	return "TutorialWorldScene";
}

void Demo::TutorialWorldScene::GenerateSaveData(nlohmann::json& outData)
{
	player->GenerateSaveData(outData["player"]);
	auto pos = camera.GetPosition();
	outData["camera"] = {
		{"x", pos.x},
		{"y", pos.y},
		{"zoom", camera.GetZoom()}
	};
}

void Demo::TutorialWorldScene::RestoreSaveData(const nlohmann::json& inData)
{
	player->RestoreSaveData(inData["player"]);
	camera.SetPosition(inData["camera"]["x"], inData["camera"]["y"]);
	camera.SetZoom(inData["camera"]["zoom"]);
}
