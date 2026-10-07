#include "pch.h"
#include "SettingsManager.h"
#include "WorldSceneBase.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include "MainMenu.h"
#include "SaveGameState.h"
#include "TransitionCommand.h"
#include "resource.h"
#include "PopupManager.h"
#include "QuestManager.h"
#include "EnemyFactory.h"
#include "MapBattleScene.h"
#include "MapView.h"
#include "RNG.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"

Demo::WorldSceneBase::WorldSceneBase(Game* game, std::shared_ptr<DX9GF::SaveManager> sm, UINT sw, UINT sh)
	: IScene(sw, sh), game(game), saveManager(sm) {
}

void Demo::WorldSceneBase::Init() {
	OnInit();
}

void Demo::WorldSceneBase::InitCore(float playerX, float playerY, const wchar_t* mapFile)
{
	camera.SetZoom(2.0f);
	transformManager = std::make_shared<DX9GF::TransformManager>();
	colliderManager = std::make_shared<DX9GF::ColliderManager>();
	player = std::make_shared<Player>(transformManager, playerX, playerY);
	camera.SetPosition(playerX, playerY);
	player->Init(game->GetGraphicsDevice(), colliderManager.get(), &camera);
	gearTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	gearTex->LoadTexture(L"assets/12x12-gold-token.png"); //TODO: Change gears asset
	player->InitGearAnim(gearTex);
	drawBuffer = std::make_shared<DX9GF::CommandBuffer>();
	commandBuffer = std::make_shared<DX9GF::CommandBuffer>();
	popUpMessage = std::make_shared<PopUpMessage>(transformManager, game);
	popUpMessage->SetLocalPosition(0.0f, 0.0f);
	popUpMessage->Init(game->GetGraphicsDevice(), &this->uiCamera);
	map = std::make_shared<DX9GF::Map>(game->GetGraphicsDevice());
	std::wstring wMapFile(mapFile);
	std::string sMapFile = DX9GF::Utils::WideToUtf8(wMapFile);
	map->Create(transformManager, colliderManager, sMapFile);

	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), Demo::kMainFontName, Demo::kMainFontSize);
	chapterTitleUI = std::make_shared<ChapterTitleUI>(font);

	mapView.Init(font.get());
	mapView.Build(*map);
	mapView.Reveal(playerX, playerY);

	auto borderTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	borderTex->LoadTexture(L"assets/popup-borders.png");
	auto uiTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	uiTex->LoadTexture(L"assets/ui.png");
	PopupManager::GetInstance()->Init(game, borderTex, uiTex, font);
	QuestManager::GetInstance()->SetVirtualResolution(game->GetVirtualWidth(), game->GetVirtualHeight());
	QuestManager::GetInstance()->Init(game->GetGraphicsDevice(), transformManager, &this->uiCamera, font);

	draggableManager = std::make_shared<Demo::DraggableManager>();
	inventoryMenu = std::make_shared<InventoryMenu>(game, player, transformManager, draggableManager, &this->uiCamera, font.get());
	inventoryMenu->Init();
	playerHUD = std::make_shared<PlayerHUD>(game, player, transformManager, &this->uiCamera, font.get());
	playerHUD->SetOnInventoryOpen([this]() {
		if (inventoryMenu && !inventoryMenu->IsOpen()) inventoryMenu->Toggle();
	});
	playerHUD->Init();

	fastTravelButton = std::make_shared<TextIconButton>(transformManager, 0, 0, 96, 32, uiTex, font.get(), L"", 3);
	fastTravelButton->SetSpriteRects(DX9GF::Utils::CreateRectsVertical(0, 0, 16, 16, 3));
	fastTravelButton->SetSliceMargins(4, 4);
	fastTravelButton->SetSpriteScale(2.f, 2.f);
	fastTravelButton->SetAutoResize(true, 16.f);
	fastTravelButton->SetTextScale(1.f, 1.f);
	fastTravelButton->SetTextColor(D3DCOLOR_XRGB(0, 0, 0));
	fastTravelButton->SetTextOutline(false);
	fastTravelButton->SetDynamicTextGetter([]() { return Tr(L"Fast Travel"); });
	fastTravelButton->SetOnReleaseLeft([this](DX9GF::ITrigger*) { OpenFastTravelMenu(); });
	fastTravelButton->Init(&this->uiCamera);
	LayoutFastTravelButton();

	map->SetAreaUpdateHandler("audio_zone_leaves", [this](const DX9GF::Map::ObjectArea&) {
		GetPlayer()->SetSurface("leaves");
	});
	map->SetAreaUpdateHandler("audio_zone_metal", [this](const DX9GF::Map::ObjectArea&) {
		GetPlayer()->SetSurface("metal");
	});

	ItemData::GetInstance()->LoadData();
}

void Demo::WorldSceneBase::Update(unsigned long long deltaTime)
{
	PopupManager::GetInstance()->SetUICamera(&this->uiCamera);

	if (!hasSeenChapterIntro && !isTransitioning) {
		hasSeenChapterIntro = true;
		if (chapterTitleUI) {
			chapterTitleUI->Show(chapterTitle, chapterSubtitle, 4.0f, 0xFFFFFFFF, 0xFFFFF200, 0x000000);
		}
	}
	if (chapterTitleUI) {
		chapterTitleUI->Update(deltaTime);
	}

	auto OpenChestWithDialogLocal = [&](std::shared_ptr<TreasureChestNPC>& chest) {
		auto given = chest->Open(player.get());
		if (given.empty()) return;
		std::wstring msg = Tr(L"You found: ");
		for (auto& r : given) {
			if (r.type == ChestRewardType::ITEM) {
				auto* bp = ItemData::GetInstance()->GetItemBlueprint(r.itemID);
				if (bp) {
					msg += bp->GetName();
					if (r.quantity > 1) msg += L" x" + std::to_wstring(r.quantity);
					msg += L"  ";
				}
			}
			else if (r.type == ChestRewardType::CARD) {
				std::wstring wid = DX9GF::Utils::Utf8ToWide(r.cardSaveID);
				msg += wid + L"  ";
			}
		}
		auto [sw, sh] = camera.GetScreenResolution();
		currentConversation = std::make_shared<IConversation>(
			std::make_shared<DX9GF::FontSprite>(font.get()), sw, sh);
		currentConversation->AddLine({ .name = Tr(L"Treasure Chest"), .content = msg, .voiceClip = std::optional<std::string>("bleep20") });
	};

	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);

	QuestManager::GetInstance()->SetUICamera(&this->uiCamera);
	QuestManager::GetInstance()->SetVirtualResolution(game->GetVirtualWidth(), game->GetVirtualHeight());
	QuestManager::GetInstance()->SetVisible(!(inventoryMenu && inventoryMenu->IsOpen()) && !IsSubsceneModalActive());
	QuestManager::GetInstance()->Update(deltaTime);
	if (QuestManager::GetInstance()->ConsumeQuestMenuRequest() && inventoryMenu) {
		inventoryMenu->SetTab(Demo::InventoryMenu::Tab::QUEST);
		if (!inventoryMenu->IsOpen()) {
			inventoryMenu->Toggle();
		}
	}
	static float escCooldown = 0.0f;
	if (escCooldown > 0) escCooldown -= deltaTime;

	// Full map: the map key toggles it; the inventory key also closes it (without opening the inventory)
	bool mapClosedThisFrame = false;
	{
		auto settings = SettingsManager::GetInstance();
		if (fullMapOpen) {
			if (inpMan->KeyDown(settings->GetKeybind("OPEN_MAP")) || inpMan->KeyDown(settings->GetKeybind("OPEN_INVENTORY"))) {
				fullMapOpen = false;
				mapClosedThisFrame = true;
			}
		}
		else if (!isTransitioning && !currentConversation && !IsSubsceneModalActive()
			&& !PopupManager::GetInstance()->IsActive() && !(inventoryMenu && inventoryMenu->IsOpen())
			&& inpMan->KeyDown(settings->GetKeybind("OPEN_MAP"))) {
			fullMapOpen = true;
		}
	}

	if (!fullMapOpen && !mapClosedThisFrame && !IsSubsceneModalActive() && inpMan->KeyDown(SettingsManager::GetInstance()->GetKeybind("OPEN_INVENTORY")) && escCooldown <= 0) {
		if (inventoryMenu) inventoryMenu->Toggle();
		escCooldown = 300.0f;
	}

	bool isGamePaused = this->isGamePaused || IsSubsceneModalActive() || fullMapOpen;

	// Pay out quests whose goal was already done before the player accepted them.
	if (!isTransitioning && !currentConversation && !PopupManager::GetInstance()->IsActive()) {
		auto deferred = QuestManager::GetInstance()->ResolveDeferredEvents(player.get());
		if (deferred.hasReward && popUpMessage) {
			popUpMessage->ShowMessage(L"(+) " + deferred.rewardMessage, 5.0f);
		}
	}

	// Announce freshly obtained gears one at a time, once any dialogue / other popup is out of the way.
	auto& pendingGears = PlayerGlobalData::GetInstance()->GetPendingGearPopups();
	if (!pendingGears.empty() && !isTransitioning && !currentConversation && !IsSubsceneModalActive()
		&& !PopupManager::GetInstance()->IsActive()) {
		int gearID = pendingGears.front();
		pendingGears.erase(pendingGears.begin());
		auto bp = ItemData::GetInstance()->GetGearBlueprint(gearID);
		if (bp) {
			std::vector<std::pair<std::wstring, std::function<void()>>> buttons = { { Tr(L"OK"), nullptr } };
			PopupManager::GetInstance()->ShowWithIcon("stepped_gold", Tr(L"Gear Obtained!"), Tr(bp->name),
				buttons, gearTex, bp->frames);
		}
	}

	if (PopupManager::GetInstance()->IsActive()) {
		PopupManager::GetInstance()->Update(deltaTime, &this->uiCamera);
		isGamePaused = true;
	}

	if (popUpMessage) {
		popUpMessage->Update(deltaTime);
	}

	for (auto& npc : mapNPCs) {
		npc->Update(deltaTime);

		if (!currentConversation && !IsSubsceneModalActive() && npc->CanInteract() && inpMan->KeyPress(SettingsManager::GetInstance()->GetKeybind("INTERACT"))) {
			npc->ClearLines();
			auto onEndCallback = npc->TriggerInteract();

			activeNPC = npc;
			activeNPC->SetOnDialogueEnd(onEndCallback);

			auto [sw, sh] = camera.GetScreenResolution();
			currentConversation = std::make_shared<IConversation>(std::make_shared<DX9GF::FontSprite>(font.get()), sw, sh);
			for (auto& line : npc->GetDialogueLines()) {
				currentConversation->AddLine(line);
			}
			break;
		}
	}

	if (currentConversation) {
		isGamePaused = true;
		currentConversation->Execute(deltaTime);

		if (currentConversation->IsFinished()) {
			if (activeNPC && activeNPC->GetOnDialogueEnd()) {
				activeNPC->GetOnDialogueEnd()();
			}

			if (onConversationEnd) {
				auto callback = std::move(onConversationEnd);
				onConversationEnd = nullptr;
				callback();
			}

			currentConversation = nullptr;
			activeNPC = nullptr;
		}
	}

	for (auto& savePoint : savePoints) {
		savePoint->Update(deltaTime);
	}
	for (auto& shopPoint : shopPoints) {
		shopPoint->Update(deltaTime);
	}
	for (auto& healingPoint : healingPoints) {
		healingPoint->Update(deltaTime);
	}

	for (auto& chest : treasureChests) {
		chest->Update(deltaTime);
		if (!currentConversation && !IsSubsceneModalActive() && chest->CanInteract() && inpMan->KeyPress(SettingsManager::GetInstance()->GetKeybind("INTERACT"))) {
			auto given = chest->Open(player.get());
			if (!given.empty()) {
				std::wstring msg = Tr(L"You found: ");
				for (auto& r : given) {
					if (r.type == ChestRewardType::ITEM) {
						auto* bp = ItemData::GetInstance()->GetItemBlueprint(r.itemID);
						if (bp) {
							msg += bp->GetName();
							if (r.quantity > 1) msg += L" x" + std::to_wstring(r.quantity);
							msg += L"  ";
						}
					}
					else if (r.type == ChestRewardType::CARD) {
						std::wstring wid = DX9GF::Utils::Utf8ToWide(r.cardSaveID);
						msg += wid + L"  ";
					}
				}
				auto [sw, sh] = camera.GetScreenResolution();
				currentConversation = std::make_shared<IConversation>(
					std::make_shared<DX9GF::FontSprite>(font.get()), sw, sh);
				currentConversation->AddLine({ .name = Tr(L"Treasure Chest"), .content = msg, .voiceClip = std::optional<std::string>("bleep20") });
			}
		}
	}

	if (inventoryMenu && inventoryMenu->IsOpen()) {
		isGamePaused = true;
		inventoryMenu->Update(deltaTime);
	}

	if (playerHUD && !isGamePaused) playerHUD->Update(deltaTime);

	if (fastTravelButton && ShowMiniMap()) {
		LayoutFastTravelButton();
		if (!isGamePaused && !isTransitioning) fastTravelButton->Update(deltaTime);
	}

	OnUpdate(deltaTime);

	if (!isGamePaused && !isTransitioning) {
		for (auto& enemy : mapEnemies) {
			enemy->Update(deltaTime);
		}
		player->Update(deltaTime);
		camera.Update();
		mapView.Reveal(player->GetWorldX(), player->GetWorldY());
	}

	this->uiCamera.Update();
	transformManager->UpdateAll();
	if (!isGamePaused) map->UpdateAreas(player->GetCollider().lock()->GetWorldX(), player->GetCollider().lock()->GetWorldY());

	if (draggableManager && inventoryMenu && inventoryMenu->IsOpen() && !inventoryMenu->IsTutorialActive()
		&& inventoryMenu->GetCurrentTab() == Demo::InventoryMenu::Tab::DECK) {
		draggableManager->Update(deltaTime);
	}

	if (inventoryMenu && inventoryMenu->IsPendingLeave()) {
		auto sceMan = game->GetSceneManager();
		sceMan->GoToScene(0);
		auto audio = DX9GF::AudioManager::GetInstance();
		audio->PlayBGM_Fade("bgm_sky", 0.9f, 1.5f);
		return;
	}
	commandBuffer->Update(deltaTime);
}

void Demo::WorldSceneBase::DrawWorld(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	if (SUCCEEDED(gd->BeginDraw())) {

		DrawBackground(gd, deltaTime);
		map->Draw(camera);

		std::vector<DepthNode> depthNodes;

		for (auto& savePoint : savePoints) {
			depthNodes.push_back({ savePoint->GetWorldY(), [&, savePoint]() { savePoint->Draw(camera, deltaTime); } });
		}
		for (auto& shopPoint : shopPoints) {
			depthNodes.push_back({ shopPoint->GetWorldY(), [&, shopPoint]() { shopPoint->Draw(camera, deltaTime); } });
		}
		for (auto& healingPoint : healingPoints) {
			depthNodes.push_back({ healingPoint->GetWorldY(), [&, healingPoint]() { healingPoint->Draw(camera, deltaTime); } });
		}
		for (auto& chest : treasureChests) {
			depthNodes.push_back({ chest->GetWorldY(), [&, chest]() { chest->Draw(camera, deltaTime); } });
		}
		for (auto& enemy : mapEnemies) {
			depthNodes.push_back({ enemy->GetWorldY(), [&, enemy]() { enemy->Draw(&camera, deltaTime); } });
		}
		for (auto& npc : mapNPCs) {
			depthNodes.push_back({ npc->GetWorldY(), [&, npc]() { npc->Draw(camera, deltaTime); } });
		}
		if (player) depthNodes.push_back({ player->GetWorldY(), [&]() { player->Draw(deltaTime); } });

		OnDrawWorld(depthNodes, deltaTime);

		std::sort(depthNodes.begin(), depthNodes.end());
		for (auto& node : depthNodes) {
			node.drawCall();
		}

		gd->EndDraw();
	}
}

void Demo::WorldSceneBase::DrawUI(unsigned long long deltaTime)
{
	CreateImGuiDebugFrame(player, game);
	auto gd = game->GetGraphicsDevice();

	if (SUCCEEDED(gd->BeginDraw())) {

		for (auto& savePoint : savePoints) savePoint->DrawUI(&this->uiCamera, deltaTime);
		for (auto& shopPoint : shopPoints) shopPoint->DrawUI(&this->uiCamera, deltaTime);
		for (auto& healingPoint : healingPoints) healingPoint->DrawUI(&this->uiCamera, deltaTime);
		for (auto& chest : treasureChests) chest->DrawUI(&this->uiCamera, deltaTime);
		for (auto& npc : mapNPCs) npc->DrawUI(&this->uiCamera, deltaTime);

		OnDrawUI(deltaTime);

		if (playerHUD) playerHUD->Draw(gd, deltaTime);
		if (ShowMiniMap() && !fullMapOpen && !(inventoryMenu && inventoryMenu->IsOpen()) && player) {
			mapView.DrawMini(gd, uiCamera, game->GetVirtualWidth(), game->GetVirtualHeight(),
				player->GetWorldX(), player->GetWorldY(), BuildMapMarkers());
			if (fastTravelButton) fastTravelButton->Draw(gd, deltaTime);
		}
		if (inventoryMenu) inventoryMenu->Draw(gd, deltaTime);
		if (draggableManager && inventoryMenu && inventoryMenu->IsOpen() && inventoryMenu->GetCurrentTab() == Demo::InventoryMenu::Tab::DECK) {
			draggableManager->Draw(deltaTime);
		}
		if (inventoryMenu) inventoryMenu->DrawKeyboardReticle(gd, deltaTime);

		if (currentConversation) {
			currentConversation->Draw(gd, &this->uiCamera, deltaTime);
		}

		QuestManager::GetInstance()->Draw(gd, &this->uiCamera, deltaTime);

		if (fullMapOpen) {
			mapView.DrawFull(gd, uiCamera, game->GetVirtualWidth(), game->GetVirtualHeight(), BuildMapMarkers(),
				SettingsManager::GetInstance()->GetKeybindDisplayName("OPEN_MAP"));
		}

		if (drawBuffer) {
			drawBuffer->Update(deltaTime);
		}

		PopupManager::GetInstance()->DrawUI(deltaTime, &this->uiCamera);

		if (!(inventoryMenu && inventoryMenu->IsInKeyboardMode())) {
			DX9GF::InputManager::GetInstance()->DrawCursor(&this->uiCamera, deltaTime);
		}

		if (popUpMessage) {
			popUpMessage->Draw(deltaTime);
		}

		if (chapterTitleUI) {
			chapterTitleUI->Draw(&this->uiCamera, deltaTime);
		}

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		gd->EndDraw();
	}
}

void Demo::WorldSceneBase::OpenChestWithDialog(std::shared_ptr<TreasureChestNPC>& chest)
{
	auto given = chest->Open(player.get());
	if (given.empty()) return;
	std::wstring msg = Tr(L"You found: ");
	for (auto& r : given) {
		if (r.type == ChestRewardType::ITEM) {
			auto* bp = ItemData::GetInstance()->GetItemBlueprint(r.itemID);
			if (bp) {
				msg += bp->GetName();
				if (r.quantity > 1) msg += L" x" + std::to_wstring(r.quantity);
				msg += L"  ";
			}
		}
		else if (r.type == ChestRewardType::CARD) {
			std::wstring wid = DX9GF::Utils::Utf8ToWide(r.cardSaveID);
			msg += wid + L"  ";
		}
	}
	auto [sw, sh] = camera.GetScreenResolution();
	currentConversation = std::make_shared<IConversation>(
		std::make_shared<DX9GF::FontSprite>(font.get()), sw, sh);
	currentConversation->AddLine({ .name = Tr(L"Treasure Chest"), .content = msg, .voiceClip = std::optional<std::string>("bleep20") });
}

std::vector<Demo::MapView::Marker> Demo::WorldSceneBase::BuildMapMarkers() const
{
	using K = MapView::MarkerKind;
	std::vector<MapView::Marker> markers;
	for (auto& p : savePoints) markers.push_back({ K::Save, p->GetWorldX(), p->GetWorldY() });
	for (auto& p : shopPoints) markers.push_back({ K::Shop, p->GetWorldX(), p->GetWorldY() });
	for (auto& p : healingPoints) markers.push_back({ K::Heal, p->GetWorldX(), p->GetWorldY() });
	for (auto& c : treasureChests) markers.push_back({ K::Chest, c->GetWorldX(), c->GetWorldY(), c->GetIsOpened() });
	for (auto& n : mapNPCs) markers.push_back({ K::Npc, n->GetWorldX(), n->GetWorldY() });
	for (const auto& layer : portalLayers) {
		for (const auto& area : map->GetAreas(layer)) {
			markers.push_back({ K::Portal, area.x + area.width / 2.f, area.y + area.height / 2.f });
		}
	}
	if (player) markers.push_back({ K::Player, player->GetWorldX(), player->GetWorldY() });
	return markers;
}

void Demo::WorldSceneBase::SetChapterTitle(const std::wstring& title, const std::wstring& subtitle) {
	chapterTitle = title;
	chapterSubtitle = subtitle;
}

void Demo::WorldSceneBase::AddDepthNode(std::vector<DepthNode>& nodes, float y, std::function<void()> drawCall)
{
	nodes.push_back({ y, std::move(drawCall) });
}

void Demo::WorldSceneBase::CreatePortalTransition(int sceneOffset, float targetX, float targetY, const char* bgm, float bgmVol)
{
	TransitionToScene(static_cast<int>(game->GetSceneManager()->GetIndex()) + sceneOffset, targetX, targetY, bgm, bgmVol);
}

void Demo::WorldSceneBase::TransitionToScene(int sceneIndex, float targetX, float targetY, const char* bgm, float bgmVol)
{
	if (isTransitioning) return;
	isTransitioning = true;

	auto transitionInCommand = std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, true);
	drawBuffer->PushCommand(transitionInCommand);
	commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this, transitionInCommand, sceneIndex, targetX, targetY, bgm, bgmVol](std::function<void(void)> markFinished) {
		if (!transitionInCommand->IsFinished()) {
			return;
		}
		auto sceMan = game->GetSceneManager();
		auto targetScene = sceMan->GetScene(static_cast<size_t>(sceneIndex));
		auto targetPlayer = MainMenu::gameSaveState->GetPlayerFromScene(targetScene);
		targetPlayer->SetLocalPosition(targetX, targetY);
		if (bgm) {
			DX9GF::AudioManager::GetInstance()->PlayBGM_Fade(bgm, bgmVol, 1.5f);
		}
		sceMan->GoToScene(sceneIndex);
		isTransitioning = false;
		markFinished();
		}));
	drawBuffer->PushCommand(std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, false));
}

void Demo::WorldSceneBase::LayoutFastTravelButton()
{
	const float virtualW = static_cast<float>(game->GetVirtualWidth());
	const float virtualH = static_cast<float>(game->GetVirtualHeight());
	const float miniLeft = virtualW / 2.f - MapView::kMiniMargin - MapView::kMiniSize;
	fastTravelButton->SetLocalPosition(
		miniLeft - 2.f - 8.f - static_cast<float>(fastTravelButton->GetWidth()),
		-virtualH / 2.f + MapView::kMiniMargin);
}

void Demo::WorldSceneBase::OpenFastTravelMenu()
{
	std::vector<std::pair<std::wstring, std::function<void()>>> buttons;
	for (const auto& dest : FastTravel::Destinations()) {
		if (!PlayerGlobalData::GetInstance()->IsFastTravelUnlocked(dest.sceneId)) continue;
		buttons.push_back({ Tr(dest.name), [this, &dest]() { FastTravelTo(dest); } });
	}
	if (buttons.empty()) {
		PopupManager::GetInstance()->Show("stepped_blue", Tr(L"Fast Travel"),
			Tr(L"Beat an area's boss to unlock it."), { { Tr(L"OK"), nullptr } });
		return;
	}
	buttons.push_back({ Tr(L"Cancel"), nullptr });
	PopupManager::GetInstance()->Show("stepped_blue", Tr(L"Fast Travel"), Tr(L"Return to the start of:"), buttons);
}

void Demo::WorldSceneBase::FastTravelTo(const FastTravel::Destination& dest)
{
	const int sceneIndex = MainMenu::gameSaveState->GetSceneIndex(dest.sceneId);
	if (sceneIndex < 0) return;
	TransitionToScene(sceneIndex, dest.startX, dest.startY, dest.bgm, dest.bgmVolume);
}

void Demo::WorldSceneBase::SpawnMapEnemy(float x, float y, std::string id,
	std::vector<std::string> types, bool isRand, bool isGlobal,
	std::function<void(DX9GF::GraphicsDevice*, unsigned long long)> bgDraw,
	int tokenChance, const std::string& questEvent, const std::string& questId)
{
	auto enemy = EnemyFactory::CreateMapEnemy(
		x, y, id, types, isRand, isGlobal, "battle_loop", bgDraw,
		transformManager, game, colliderManager.get(), player
	);

	Demo::EventType generatedEvent = Demo::EventType::None;
	if (static_cast<int>(Demo::RNG::Range(1, 100)) <= tokenChance) {
		if (enemy->GetEncounterData().enemyTypes.size() >= 2) {
			generatedEvent = (Demo::RNG::Range(1, 100) <= 50) ? Demo::EventType::Gold : Demo::EventType::Energy;
		}
		else {
			generatedEvent = Demo::EventType::Gold;
		}
	}
	enemy->SetEventState(generatedEvent);

	enemy->SetOnEncounterTriggered([this, questEvent, questId](std::shared_ptr<MapEnemy> e) {
		if (this->isTransitioning) return;
		this->isTransitioning = true;

		auto transitionIn = std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, true);
		this->drawBuffer->PushCommand(transitionIn);

		this->commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this, transitionIn, e, questEvent, questId](std::function<void(void)> markFinished) {
			if (!transitionIn->IsFinished()) return;

			auto app = DX9GF::Application::GetInstance();
			auto sceMan = this->game->GetSceneManager();
			auto battleScene = new MapBattleScene(this->game, this->player, app->GetScreenWidth(), app->GetScreenHeight(), e->GetEncounterData());

			battleScene->SetOnVictoryCallback([e, questEvent, questId, this]() {
				e->SetDefeatedState(true, 180.f);

				// Complete the tutorial quest the first time the player defeats an enemy.
				auto firstEncounter = QuestManager::GetInstance()->NotifyEvent("FIRST_ENCOUNTER_DEFEATED", "", this->player.get());
				if (firstEncounter.hasReward && this->popUpMessage) {
					this->popUpMessage->ShowMessage(L"(+) " + firstEncounter.rewardMessage, 5.0f);
				}

				if (!questEvent.empty()) {
					auto result = QuestManager::GetInstance()->NotifyEvent(questEvent, questId, this->player.get());
					if (result.hasReward && this->popUpMessage) {
						this->popUpMessage->ShowMessage(L"(+) " + result.rewardMessage, 5.0f);
					}
				}
			});

			sceMan->InsertScene(sceMan->GetIndex() + 1, battleScene);
			sceMan->GoToNext();

			this->isTransitioning = false;
			markFinished();
		}));

		this->drawBuffer->PushCommand(std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, false));
	});

	mapEnemies.push_back(enemy);
}

void Demo::WorldSceneBase::GenerateSaveData(nlohmann::json& outData)
{
	player->GenerateSaveData(outData["player"]);
	auto pos = camera.GetPosition();
	outData["camera"] = {
		{"x", pos.x},
		{"y", pos.y},
		{"zoom", camera.GetZoom()}
	};
	outData["hasSeenChapterIntro"] = hasSeenChapterIntro;
	mapView.Save(outData["mapFog"]);

	nlohmann::json chestStates = nlohmann::json::array();
	for (auto& c : treasureChests) chestStates.push_back(c->GetIsOpened());
	outData["treasureChests"] = chestStates;

	nlohmann::json enemiesState = nlohmann::json::object();
	for (auto& enemy : mapEnemies) {
		enemiesState[enemy->GetEnemyID()] = {
			{"isDefeated", enemy->IsDefeated()},
			{"respawnTimer", enemy->GetRespawnTimer()},
			{"eventType", static_cast<int>(enemy->GetEncounterData().eventType)},
			{"enemyTypes", enemy->GetEncounterData().enemyTypes},
			{"enemyHps", enemy->GetEncounterData().enemyHps}
		};
	}
	outData["mapEnemies"] = enemiesState;

	OnGenerateSaveData(outData);
}

void Demo::WorldSceneBase::RestoreSaveData(const nlohmann::json& inData)
{
	player->RestoreSaveData(inData["player"]);
	camera.SetPosition(inData["camera"]["x"], inData["camera"]["y"]);
	camera.SetZoom(inData["camera"]["zoom"]);
	hasSeenChapterIntro = inData.value("hasSeenChapterIntro", false);
	if (inData.contains("mapFog")) mapView.Restore(inData["mapFog"]);

	if (inData.contains("treasureChests")) {
		auto& arr = inData["treasureChests"];
		for (size_t i = 0; i < treasureChests.size() && i < arr.size(); ++i)
			treasureChests[i]->SetOpened(arr[i].get<bool>());
	}

	if (inData.contains("mapEnemies")) {
		auto& enemiesState = inData["mapEnemies"];
		for (auto& enemy : mapEnemies) {
			std::string id = enemy->GetEnemyID();
			if (enemiesState.contains(id)) {
				bool def = enemiesState[id]["isDefeated"].get<bool>();
				float timer = enemiesState[id]["respawnTimer"].get<float>();
				EventType savedEvent = static_cast<EventType>(enemiesState[id].value("eventType", 0));

				enemy->SetDefeatedState(def, timer);
				enemy->SetEventState(savedEvent);

				// Older saves have no roll; those enemies simply keep the one made when the scene was built.
				if (enemiesState[id].contains("enemyTypes") && enemiesState[id].contains("enemyHps")) {
					enemy->SetEncounterRoll(
						enemiesState[id]["enemyTypes"].get<std::vector<std::string>>(),
						enemiesState[id]["enemyHps"].get<std::vector<int>>());
				}
			}
		}
	}

	OnRestoreSaveData(inData);
}
