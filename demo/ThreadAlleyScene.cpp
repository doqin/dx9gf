#include "pch.h"
#include "ThreadAlleyScene.h"
#include "RandomEncounter.h"
#include "CardShop.h"
#include "ItemShop.h"

void Demo::ThreadAlleyScene::Init()
{
	camera.SetZoom(2.0f);
	transformManager = std::make_shared<DX9GF::TransformManager>();
	colliderManager = std::make_shared<DX9GF::ColliderManager>();

	player = std::make_shared<Player>(transformManager, -544.5f, 128.5f);
	camera.SetPosition(-544.5f, 128.5f);

	player->Init(game->GetGraphicsDevice(), colliderManager.get(), &camera);

	drawBuffer = std::make_shared<DX9GF::CommandBuffer>();
	commandBuffer = std::make_shared<DX9GF::CommandBuffer>();

	map = std::make_shared<DX9GF::Map>(game->GetGraphicsDevice());
	map->Create(transformManager, colliderManager, "./ThreadAlley.tmx");

	map->SetAreaUpdateHandler("triggers", GetRandomEncounterFunc(game, player, {
		{"DemonEyeEnemy", 40},
		{"VampireBatEnemy", 30},
		{"MimicEnemy", 20},
		{"WarlockEnemy", 10},
		{"CupidEnemy", 5}
		}, drawBuffer, commandBuffer, &isGamePaused));

	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"StatusPlz", 16);

	shopPoints.push_back(std::make_shared<ShopPoint>(transformManager, -710, -450));
	shopPoints.back()->Init(game, game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer,
       [](Game* g, Player* p, int w, int h) { return new CardShop(g, p, w, h, ShopTier::HYBRID); }
	);
	shopPoints.back()->SetVisible(true);

	shopPoints.push_back(std::make_shared<ShopPoint>(transformManager, -697, -1112));
	shopPoints.back()->Init(game, game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer,
       [](Game* g, Player* p, int w, int h) { return new ItemShop(g, p, w, h, ShopTier::HYBRID); }
	);
	shopPoints.back()->SetVisible(true);

	shopPoints.push_back(std::make_shared<ShopPoint>(transformManager, 35, -1413));
	shopPoints.back()->Init(game, game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer,
       [](Game* g, Player* p, int w, int h) { return new CardShop(g, p, w, h, ShopTier::BASIC); }
	);
	shopPoints.back()->SetVisible(true);

	shopPoints.push_back(std::make_shared<ShopPoint>(transformManager, 230, -456));
	shopPoints.back()->Init(game, game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer,
       [](Game* g, Player* p, int w, int h) { return new ItemShop(g, p, w, h, ShopTier::BASIC); }
	);
	shopPoints.back()->SetVisible(true);
	  
	//

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, -710, -554));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, -474, -137));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, -727, -1192));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, -235, -1302));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, 134, -1129));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, 824, -969));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, 582, -1036));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	//savePoints.push_back(std::make_shared<SavePoint>(transformManager, 230, -392));
	//savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	//savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, 968, -216));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	savePoints.push_back(std::make_shared<SavePoint>(transformManager, 1210, -91));
	savePoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, saveManager, font, drawBuffer);
	savePoints.back()->SetVisible(true);

	//

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, -328, -537));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, -538, -137));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	/*healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, -456, -1016));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);*/

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, -634, -1273));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, -59, -1356));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 407, -1337));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 582, -841));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	/*healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 984, -839));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);*/

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 767, -1041));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	/*healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 920, -1351));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);*/

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 1257, -1351));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	//healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 233, -297));
	//healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	//healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 585, -539));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	/*healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 584, -123));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);*/

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 872, -262));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	/*healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 935, -616));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);*/

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 1174, -1143));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);

	healingPoints.push_back(std::make_shared<HealingPoint>(transformManager, 1174, -441));
	healingPoints.back()->Init(game->GetGraphicsDevice(), &camera, player, colliderManager, font, drawBuffer);
	healingPoints.back()->SetVisible(true);


	draggableManager = std::make_shared<Demo::DraggableManager>();
	inventoryMenu = std::make_shared<InventoryMenu>(game, player, transformManager, draggableManager, &uiCamera, font.get());
	inventoryMenu->Init();

	ItemData::GetInstance()->LoadData();
	this->GiveTestItems();

	transformManager->RebuildHierarchy();
}

void Demo::ThreadAlleyScene::Update(unsigned long long deltaTime)
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

	bool isGamePaused = this->isGamePaused;

	if (!isGamePaused) map->UpdateAreas(player->GetWorldX(), player->GetWorldY());

	for (auto& savePoint : savePoints) {
		savePoint->Update(deltaTime);
		if (savePoint->IsMenuOpen()) isGamePaused = true;
	}

	for (auto& shopPoint : shopPoints) {
		shopPoint->Update(deltaTime);
	}

	for (auto& healPoint : healingPoints) {
		healPoint->Update(deltaTime);
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
		sceMan->GoToScene(0);
		return;
	}
	commandBuffer->Update(deltaTime);
}

void Demo::ThreadAlleyScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear(0xFFFFFFFF);
	if (SUCCEEDED(gd->BeginDraw())) {
		map->Draw(camera);

		for (auto& savePoint : savePoints) {
			savePoint->Draw(camera, deltaTime);
		}

		for (auto& shopPoint : shopPoints) {
			shopPoint->Draw(camera, deltaTime);
		}

		for (auto& healPoint : healingPoints) {
			healPoint->Draw(camera, deltaTime);
		}

		player->Draw(deltaTime);

		if (drawBuffer) {
			drawBuffer->Update(deltaTime);
		}
		if (inventoryMenu) inventoryMenu->Draw(gd, deltaTime);
		if (draggableManager && inventoryMenu && inventoryMenu->IsOpen() && inventoryMenu->GetCurrentTab() == Demo::InventoryMenu::Tab::DECK) {
			draggableManager->Draw(deltaTime);
		}

		DX9GF::InputManager::GetInstance()->DrawCursor(&this->uiCamera, deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}

std::string Demo::ThreadAlleyScene::GetSaveID() const
{
	return "ThreadAlleyScene";
}

void Demo::ThreadAlleyScene::GenerateSaveData(nlohmann::json& outData)
{
	player->GenerateSaveData(outData["player"]);
	auto pos = camera.GetPosition();
	outData["camera"] = {
		{"x", pos.x},
		{"y", pos.y},
		{"zoom", camera.GetZoom()}
	};
}

void Demo::ThreadAlleyScene::RestoreSaveData(const nlohmann::json& inData)
{
	player->RestoreSaveData(inData["player"]);
	camera.SetPosition(inData["camera"]["x"], inData["camera"]["y"]);
	camera.SetZoom(inData["camera"]["zoom"]);
}

void Demo::ThreadAlleyScene::GiveTestItems()
{

}