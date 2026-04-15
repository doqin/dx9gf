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

	savePoint = std::make_shared<SavePoint>(transformManager, 200.0f, 150.0f);
	savePoint->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font);
	savePoint->SetVisible(true);

	draggableManager = std::make_shared<Demo::DraggableManager>();
	auto card1 = std::make_shared<Demo::StrikeCard>(transformManager, 150, 40);
	card1->Init(draggableManager, game->GetGraphicsDevice(), &uiCamera);
	player->AddCardToDeck(card1);

	auto card2 = std::make_shared<Demo::StrikeCard>(transformManager, 150, 40);
	card2->Init(draggableManager, game->GetGraphicsDevice(), &uiCamera);
	player->AddCardToDeck(card2);

	auto card3 = std::make_shared<Demo::StrikeCard>(transformManager, 150, 40);
	card3->Init(draggableManager, game->GetGraphicsDevice(), &uiCamera);
	player->AddCardToInventory(card3);
	inventoryMenu = std::make_shared<InventoryMenu>(game, player, transformManager, draggableManager, &uiCamera, font.get());
	inventoryMenu->Init();
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

	static float escCooldown = 0.0f;
	if (escCooldown > 0) escCooldown -= deltaTime;

	if (inpMan->KeyPress(DIK_ESCAPE) && escCooldown <= 0) {
		if (inventoryMenu) inventoryMenu->Toggle();
		escCooldown = 300.0f;
	}

	bool isGamePaused = false;

	if (savePoint) {
		savePoint->Update(deltaTime);
		if (savePoint->IsMenuOpen()) isGamePaused = true;
	}

	if (inventoryMenu && inventoryMenu->IsOpen()) {
		isGamePaused = true;
		inventoryMenu->Update(deltaTime);
	}

	if (!isGamePaused) {
		player->Update(deltaTime);
		camera.Update();
	}

	transformManager->UpdateAll();


	if (draggableManager && inventoryMenu && inventoryMenu->IsOpen() && inventoryMenu->GetCurrentTab() == Demo::InventoryMenu::Tab::DECK) {
		draggableManager->Update(deltaTime);
	}


	if (inventoryMenu && inventoryMenu->IsPendingLeave()) {
		auto sceMan = game->GetSceneManager();
		sceMan->PopScene();
		sceMan->GoToPrevious();
		return;
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
		if (inventoryMenu) inventoryMenu->Draw(gd, deltaTime);
		if (draggableManager && inventoryMenu && inventoryMenu->IsOpen() && inventoryMenu->GetCurrentTab() == Demo::InventoryMenu::Tab::DECK) {
			draggableManager->Draw(deltaTime);
		}
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
