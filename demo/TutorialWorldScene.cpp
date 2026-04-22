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

	npc1 = std::make_shared<NPC1>(transformManager, 250.0f, 120.0f);
	npc1->Init(game->GetGraphicsDevice(), player, colliderManager, font);
	npc1->AddLine(L"NPC", L"Hello! Welcome.");
	npc1->AddLine(L"NPC", L"How to escape this world? I don't know.");

	savePoint = std::make_shared<SavePoint>(transformManager, 200.0f, 150.0f);
	savePoint->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font);
	savePoint->SetVisible(true);

	shopPoint = std::make_shared<ShopPoint>(transformManager, 300.0f, 150.0f);
	shopPoint->Init(game, game->GetGraphicsDevice(), &camera, player, colliderManager, font);
	shopPoint->SetVisible(true);

	healingPoint = std::make_shared<HealingPoint>(transformManager, 250.0f, 220.0f);
	healingPoint->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font);
	healingPoint->SetVisible(true);

	draggableManager = std::make_shared<Demo::DraggableManager>();
	inventoryMenu = std::make_shared<InventoryMenu>(game, player, transformManager, draggableManager, &uiCamera, font.get());
	inventoryMenu->Init();

	ItemData::GetInstance()->LoadData();
	this->GiveTestItems();

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

	if (npc1) {
		npc1->Update(deltaTime);
		if (!currentConversation && npc1->CanInteract() && inpMan->KeyPress(DIK_E)) {
			auto [sw, sh] = camera.GetScreenResolution();
			currentConversation = std::make_shared<IConversation>(std::make_shared<DX9GF::FontSprite>(font.get()), sw, sh);
			for (auto& line : npc1->GetDialogueLines()) {
				currentConversation->AddLine(line);
			}
		}
	}

	if (currentConversation) {
		isGamePaused = true;
		currentConversation->Execute(deltaTime);
		if (currentConversation->IsFinished()) currentConversation = nullptr;
	}

	if (savePoint) {
		savePoint->Update(deltaTime);
		if (savePoint->IsMenuOpen()) isGamePaused = true;
	}

	if (shopPoint) shopPoint->Update(deltaTime);

	if (healingPoint) healingPoint->Update(deltaTime);

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
		if (npc1) npc1->Draw(camera, deltaTime);
		if (currentConversation) currentConversation->Draw(gd, deltaTime);
		if (savePoint) {
			savePoint->Draw(camera, deltaTime);
		}
		if (shopPoint) shopPoint->Draw(camera, deltaTime);
		if (healingPoint) healingPoint->Draw(camera, deltaTime);
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

void Demo::TutorialWorldScene::GiveTestItems()
{
	ItemInventory& testItems = this->player->GetInventoryItems();
	testItems.InitFixedInventory(10);

	testItems.AddItem(0, 5);
	testItems.AddItem(1, 3);
	testItems.AddItem(2, 2);
	testItems.AddItem(3, 1);
	testItems.AddItem(4, 1);
	testItems.AddItem(5, 1);
	testItems.AddItem(6, 1);
	testItems.AddItem(7, 1);
	testItems.AddItem(8, 1);
	testItems.AddItem(9, 1);
}
