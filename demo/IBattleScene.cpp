#include "pch.h"
#include "IBattleScene.h"
#include <algorithm>
#include <random>
#include "DamageTextManager.h"
#include "GameItems.h"
namespace {
	constexpr float HiddenPileX = -10000.f;
	constexpr float HiddenPileY = -10000.f;

	void HidePileCard(const std::shared_ptr<Demo::ICard>& card)
	{
		if (!card) {
			return;
		}
		if (auto parent = card->GetParent(); parent.has_value()) {
			dynamic_pointer_cast<Demo::IDraggable>(card)->DetachParent();
		}
		card->SetLocalPosition(HiddenPileX, HiddenPileY);
	}
}

void Demo::IBattleScene::StartBattle()
{
	for (auto& card : drawPile) {
		HidePileCard(card);
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(drawPile.begin(), drawPile.end(), gen);
	currentTurn = 1;
	DrawCards(5);
}

void Demo::IBattleScene::DrawCards(size_t count)
{
	auto app = DX9GF::Application::GetInstance();
	auto screenWidth = app->GetScreenWidth();
	auto screenHeight = app->GetScreenHeight();
	auto x = -static_cast<float>(screenWidth) / 2.f + 20.f;
	auto y = -static_cast<float>(screenHeight) / 2.f + 20.f;
	for (size_t i = 0; i < count; ++i) {
		if (drawPile.empty()) {
			ShuffleDiscardIntoDrawPile();
		}
		if (drawPile.empty()) {
			break;
		}
		auto card = drawPile.back();
		drawPile.pop_back();
		dynamic_pointer_cast<IDraggable>(card)->DetachParent();
		y += 30;
		std::vector<std::shared_ptr<DX9GF::ICommand>> commands = {
			std::make_shared<DX9GF::SetPositionCommand>(card, -static_cast<float>(screenWidth), y),
			std::make_shared<DX9GF::CustomCommand>([this, card](std::function<void(void)> markFinished) {
				this->queuedToDraw.push_back(card);
				markFinished();
			}),
			std::make_shared<DX9GF::DelayCommand>(i * .2f),
			std::make_shared<DX9GF::GoToCommand>(card, x, y, 0.5f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}),
			std::make_shared<DX9GF::DelayCommand>(0.75f),
			std::make_shared<DX9GF::GoToCommand>(card, x, screenHeight, 0.75f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}),
			std::make_shared<DX9GF::CustomCommand>([this, card](std::function<void(void)> markFinished) {
				this->queuedToDraw.erase(std::find(queuedToDraw.begin(), queuedToDraw.end(), card));
				cardHand.push_back(card);
				if (handContainer) {
					handContainer->StoreCard(card);
				}
				markFinished();
			})
		};
		commandBuffer.StackCommand(std::make_shared<DX9GF::MultiCommand>(std::move(commands)));
	}
}

void Demo::IBattleScene::ShuffleDiscardIntoDrawPile()
{
	if (discardPile.empty()) {
		return;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(discardPile.begin(), discardPile.end(), gen);
	for (auto& card : discardPile) {
		HidePileCard(card);
		drawPile.push_back(card);
	}
	discardPile.clear();
}

void Demo::IBattleScene::MovePlayedPileToDiscardPileIfNeeded()
{
	if (currentTurn == 0 || currentTurn % 3 != 1 || playedPile.empty()) {
		return;
	}
	for (size_t i = 0; i < enemyCards.size(); ++i) {
		auto& enemyCard = enemyCards[i];
		if (auto manager = enemyCard->GetDraggableManager().lock()) {
			manager->Remove(enemyCard);
		}
		enemyCards.erase(enemyCards.begin() + i);
		--i;
	}
	for (auto& card : playedPile) {
		HidePileCard(card);
		discardPile.push_back(card);
	}
	playedPile.clear();
}

void Demo::IBattleScene::MoveExecutedHandCardsToPlayedPile()
{
	for (size_t i = 0; i < cardHand.size(); ++i) {
		auto parent = cardHand[i]->GetParent();
		if (parent.has_value() && parent.value().lock().get() == handContainer.get()) {
			continue;
		}
		playedPile.push_back(cardHand[i]);
		cardHand.erase(cardHand.begin() + i);
		--i;
	}
}

void Demo::IBattleScene::MoveHandCardsToDiscardPile()
{
	for (size_t i = 0; i < cardHand.size(); ++i) {
		HidePileCard(cardHand[i]);
		discardPile.push_back(cardHand[i]);
		cardHand.erase(cardHand.begin() + i);
		--i;
	}
}

void Demo::IBattleScene::BeginNextTurn()
{
	battlePlayer->UpdateBuffs();
	++currentTurn;
	MovePlayedPileToDiscardPileIfNeeded();
	DrawCards(5);
	energy = MAX_ENERGY;
	usedEnergy = 0;
}

void Demo::IBattleScene::QueueEnemyLayoutTransition(State targetState)
{
	if (enemies.empty()) {
		return;
	}

	const auto app = DX9GF::Application::GetInstance();
	const float centerLineY = -120.f;
	const float horizontalSpacing = 120.f;
	const float verticalSpacing = 100.f;
	const float rightSideX = app->GetScreenWidth() / 2.f - 150.f;

	bool hasQueued = false;
	const size_t enemyCount = enemies.size();
	for (size_t i = 0; i < enemyCount; ++i) {
		float targetX = 0.f;
		float targetY = 0.f;

		if (targetState == State::PlayerStandBy) {
			const float totalWidth = (enemyCount > 1) ? (enemyCount - 1) * horizontalSpacing : 0.f;
			targetX = -totalWidth / 2.f + i * horizontalSpacing;
			targetY = centerLineY;
		}
		else if (targetState == State::EnemyAttack) {
			const float totalHeight = (enemyCount > 1) ? (enemyCount - 1) * verticalSpacing : 0.f;
			targetX = app->GetScreenWidth() / 1.5f;
			targetY = -totalHeight / 2.f + i * verticalSpacing;
		}
		else {
			const float totalHeight = (enemyCount > 1) ? (enemyCount - 1) * verticalSpacing : 0.f;
			targetX = rightSideX;
			targetY = -totalHeight / 2.f + i * verticalSpacing;
		}

		auto command = std::make_shared<DX9GF::GoToCommand>(enemies[i], targetX, targetY, 0.4f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{});
		if (!hasQueued) {
			commandBuffer.PushCommand(command);
			hasQueued = true;
		}
		else {
			commandBuffer.StackCommand(command);
		}
	}
}

void Demo::IBattleScene::CreateEnemyCard(std::shared_ptr<IEnemy> enemy)
{
	auto card = std::make_shared<EnemyCard>(transformManager, enemy, enemy->GetWorldX(), enemy->GetWorldY());
	card->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	enemyCards.push_back(card);
}

void Demo::IBattleScene::RemoveEnemyCardsInRemoveArea()
{
	const float left = enemyCardRemoveAreaX;
	const float top = enemyCardRemoveAreaY;
	const float right = enemyCardRemoveAreaX + enemyCardRemoveAreaWidth;
	const float bottom = enemyCardRemoveAreaY + enemyCardRemoveAreaHeight;

	enemyCards.erase(std::remove_if(enemyCards.begin(), enemyCards.end(), [&](const std::shared_ptr<EnemyCard>& enemyCard) {
		if (!enemyCard || enemyCard->IsDragging()) {
			return false;
		}
		auto [x, y] = enemyCard->GetWorldPosition();
		if (x >= left && x <= right && y >= top && y <= bottom) {
			if (auto manager = enemyCard->GetDraggableManager().lock()) {
				manager->Remove(enemyCard);
			}
			return true;
		}
		return false;
		}), enemyCards.end());
}

void Demo::IBattleScene::RefreshItemMenu()
{
	buffItems.clear();
	auto& inventory = player->GetInventoryItems().GetSlots();

	const auto rawItemWidth = 23.0f;
	const auto rawItemHeight = 35.0f;
	const auto rawMenuBgWidth = 192.0f;
	const auto rawMenuBgHeight = 128.0f;

	//actual size (scaled)
	float itemWidth = rawItemWidth * 2.0f;
	float itemHeight = rawItemHeight * 2.0f;
	float menuBgWidth = rawMenuBgWidth * 3.0f;
	float menuBgHeight = rawMenuBgHeight * 3.0f;

	float paddingX = 20.0f;
	float paddingY = 30.0f;

	float halfBgWidth = (menuBgWidth) / 2.0f;
	float halfBgHeight = (menuBgHeight) / 2.0f;

	int columns = std::floor((menuBgWidth - paddingX) / (itemWidth + paddingX));
	if (columns < 1) columns = 1;

	float totalGridWidth = (columns * itemWidth) + ((columns - 1) * paddingX);
	float startX = -halfBgWidth + (menuBgWidth - totalGridWidth) / 2.0f;
	float startY = -halfBgHeight + paddingY;

	int displayIndex = 0;
	for (int i = 0; i < inventory.size(); i++)
	{
		auto slot = inventory[i];

		if (slot.quantity <= 0) continue;

		const auto* blueprint = Demo::ItemData::GetInstance()->GetItemBlueprint(slot.itemID);
		if (!blueprint) continue;

		int col = displayIndex % columns;
		int row = displayIndex / columns;

		float baseX = startX + col * (itemWidth + paddingX);
		float baseY = startY + row * (itemHeight + paddingY);

		auto btn = std::make_shared<IconButton>(transformManager, 0, 0, itemWidth, itemHeight, tempTex, 1);
		btn->Init(&camera);
		btn->SetSpriteScale(2.f, 2.f);
		btn->SetLocalPosition(baseX, baseY);
		btn->Update(0);

		btn->SetSpriteRects({ blueprint->GetItemRect() });

		btn->SetOnReleaseLeft([&, slot, blueprint](DX9GF::ITrigger* thisObj) {
			commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&, slot, blueprint](std::function<void(void)> markFinished) {

				if (player->GetInventoryItems().ConsumeItem(slot.itemID)) {

					for (auto& buff : blueprint->GetBuffs()) {
						if (buff.type == Demo::ItemBuffType::HealHP) {
							battlePlayer->Heal(buff.value);
						}
						else {
							battlePlayer->AddActiveBuff(buff);
						}
					}

					std::wstring msg = L"Used " + blueprint->GetName() + L"!";
					popUpMessage->QueueMessage(&commandBuffer, msg);

					this->RefreshItemMenu();
					this->state = State::EnemyAttack;
				}
				markFinished();
				}));
			});

		buffItems.push_back(btn);
		displayIndex++;
	}
	transformManager->RebuildHierarchy();
	transformManager->UpdateAll();
}

void Demo::IBattleScene::PlayerStandByUpdate(unsigned long long deltaTime)
{
	auto app = DX9GF::Application::GetInstance();
	// Set position so that it's updated if screen is resized
	const float buttonY = app->GetScreenHeight() / 2.f - 50 - attackButton->GetHeight();
	const float spacing = 20.f;
	const float totalButtonsWidth = attackButton->GetWidth() + actionButton->GetWidth() + itemsButton->GetWidth() + fleeButton->GetWidth() + 3 * spacing;
	const float leftX = -totalButtonsWidth / 2;

	attackButton->SetLocalPosition(leftX, buttonY);
	actionButton->SetLocalPosition(leftX + attackButton->GetWidth() + spacing, buttonY);
	itemsButton->SetLocalPosition(leftX + attackButton->GetWidth() + spacing + actionButton->GetWidth() + spacing, buttonY);
	fleeButton->SetLocalPosition(leftX + attackButton->GetWidth() + spacing + actionButton->GetWidth() + spacing + itemsButton->GetWidth() + spacing, buttonY);

	fleeButton->Update(deltaTime);
	itemsButton->Update(deltaTime);
	actionButton->Update(deltaTime);
	attackButton->Update(deltaTime);
	for (auto& enemy : enemies) {
		enemy->Update(deltaTime);
	}
}

void Demo::IBattleScene::PlayerAttackUpdate(unsigned long long deltaTime)
{
	auto app = DX9GF::Application::GetInstance();
	const float screenWidth = static_cast<float>(app->GetScreenWidth());
	const float screenHeight = static_cast<float>(app->GetScreenHeight());
	handContainer->SetLocalPosition(-screenWidth / 2.f + 20.f, -screenHeight / 2.f + 20.f);
	usedEnergy = 0;
	for (auto& card : cardHand) {
		if (auto parent = card->GetParent(); parent.has_value()) {
			if (auto lock = parent.value().lock(); lock && lock == handContainer) {
				continue;
			}
		}
		usedEnergy += card->GetCost();
	}
	const float buttonY = screenHeight / 2.f - 20 - attackButton->GetHeight();
	const float sidePadding = 20.f;
	const float leftX = -screenWidth / 2.f + sidePadding;
	const float executeX = leftX + backButton->GetWidth() + sidePadding;

	backButton->SetLocalPosition(leftX, buttonY);
	executeButton->SetLocalPosition(executeX, buttonY);
	enemyCardRemoveAreaX = executeX + executeButton->GetWidth() + sidePadding;
	enemyCardRemoveAreaY = buttonY;
	enemyCardRemoveAreaWidth = screenWidth / 2.f - sidePadding - enemyCardRemoveAreaX;
	enemyCardRemoveAreaHeight = backButton->GetHeight();
	if (!mainBlockCard->IsExecuting()) {
		// When finished executing attack
		if (isExecutingAttacks) {
			isTransitioning = true;
			isExecutingAttacks = false;
			std::vector<std::shared_ptr<DX9GF::ICommand>> commands = {
				std::make_shared<DX9GF::DelayCommand>(1.0f),
				std::make_shared<DX9GF::CustomCommand>([this, deltaTime](std::function<void(void)> markFinished) {
					this->MoveExecutedHandCardsToPlayedPile();
					this->MoveHandCardsToDiscardPile();
					// Remove unused enemy cards or enemy cards of dead enemies
					for (size_t i = 0; i < this->enemyCards.size(); ++i) {
						auto& enemyCard = this->enemyCards[i];
						if (!enemyCard->GetParent().has_value() || enemyCard->GetValue()->IsDead()) {
							if (auto manager = enemyCard->GetDraggableManager().lock()) {
								manager->Remove(enemyCard);
							}
							this->enemyCards.erase(this->enemyCards.begin() + i);
							--i;
						}
					}
					// Remove dead enemies
					for (size_t i = 0; i < this->enemies.size(); ++i) {
						if (this->enemies[i]->IsDead()) {
							this->enemies.erase(this->enemies.begin() + i);
							--i;
						}
					}
					this->state = State::EnemyAttack;
					this->QueueEnemyLayoutTransition(State::EnemyAttack);
					for (auto& enemy : this->enemies) {
						enemy->SetState(true);
					}

					for (auto& enemy : this->enemies) {
						bool isStunned = enemy->HasStatus(StatusType::STUN);
						enemy->TickStatuses();
						if (enemy->IsDead()) {
							continue;
						}
						if (isStunned) {
							continue;
						}
						enemy->StartAttack(this->battlePlayer);
					}

					for (size_t i = 0; i < this->enemyCards.size(); ++i) {
						auto& enemyCard = this->enemyCards[i];
						if (!enemyCard->GetParent().has_value() || enemyCard->GetValue()->IsDead()) {
							if (auto manager = enemyCard->GetDraggableManager().lock()) {
								manager->Remove(enemyCard);
							}
							this->enemyCards.erase(this->enemyCards.begin() + i);
							--i;
						}
					}
					for (size_t i = 0; i < this->enemies.size(); ++i) {
						if (this->enemies[i]->IsDead()) {
							this->enemies.erase(this->enemies.begin() + i);
							--i;
						}
					}
					this->battlePlayer->SetLocalPosition(0, 0);
					this->EnemyAttackUpdate(deltaTime);
					this->isTransitioning = false;
					markFinished();
				})
			};
			commandBuffer.PushCommand(std::make_shared<DX9GF::MultiCommand>(std::move(commands)));
			return;
		}
		if (!isTransitioning) {
			backButton->Update(deltaTime);
			executeButton->Update(deltaTime);
		}
	}
	for (auto& enemy : enemies) {
		enemy->Update(deltaTime);
	}
	if (!isTransitioning) {
		draggableManager->Update(deltaTime);
		RemoveEnemyCardsInRemoveArea();
	}
}

void Demo::IBattleScene::PlayerOpenItemsUpdate(unsigned long long deltaTime)
{
	//Calculate the coordinates to display the close button exactly in the top-right corner of the menu - fixed size
	const auto rawMenuBgWidth = 192.0f;
	const auto rawMenuBgHeight = 128.0f;
	float menuBgWidth = rawMenuBgWidth * 3.0f;
	float menuBgHeight = rawMenuBgHeight * 3.0f;
	float halfBgWidth = (menuBgWidth) / 2.0f;
	float halfBgHeight = (menuBgHeight) / 2.0f;

	float closeX = halfBgWidth - closeItemMenuButton->GetWidth();
	float closeY = -halfBgHeight;

	closeItemMenuButton->SetLocalPosition(closeX, closeY);
	closeItemMenuButton->Update(deltaTime);

	for (auto& btn : buffItems) {
		btn->Update(deltaTime);
	}
}

void Demo::IBattleScene::EnemyAttackUpdate(unsigned long long deltaTime)
{
	battlePlayer->Update(deltaTime);
	bool isDoneAttacking = true;
	for (auto& enemy : enemies) {
		enemy->Update(deltaTime);
		isDoneAttacking &= enemy->IsDoneAttacking();
	}
	if (isDoneAttacking) {
		BeginNextTurn();
		state = State::PlayerStandBy;
		PlayerStandByUpdate(deltaTime);
	}
}

void Demo::IBattleScene::PlayerStandByDraw(unsigned long long deltaTime)
{
	fleeButton->Draw(game->GetGraphicsDevice(), deltaTime);
	itemsButton->Draw(game->GetGraphicsDevice(), deltaTime);
	actionButton->Draw(game->GetGraphicsDevice(), deltaTime);
	attackButton->Draw(game->GetGraphicsDevice(), deltaTime);
}

void Demo::IBattleScene::PlayerAttackDraw(unsigned long long deltaTime)
{
	game->GetGraphicsDevice()->DrawRectangle(
		camera,
		enemyCardRemoveAreaX,
		enemyCardRemoveAreaY,
		enemyCardRemoveAreaWidth,
		enemyCardRemoveAreaHeight,
		D3DXCOLOR(0.7f, 0.1f, 0.1f, 0.5f),
		true
	);
	fontSprite->Begin();
	fontSprite->SetPosition(enemyCardRemoveAreaX + 8.f, enemyCardRemoveAreaY + 8.f);
	fontSprite->SetText(L"Drop EnemyCard Here");
	fontSprite->Draw(camera, deltaTime);
	fontSprite->SetPosition(backButton->GetWorldX() + 32.f, backButton->GetWorldY() - 30.f);
	fontSprite->SetText(std::to_wstring(energy - usedEnergy) + L"/" + std::to_wstring(MAX_ENERGY));
	fontSprite->Draw(camera, deltaTime);
	fontSprite->SetPosition(executeButton->GetWorldX() + 32.f, executeButton->GetWorldY() - 30.f);
	fontSprite->SetText(std::to_wstring(3 - (currentTurn - 1) % 3) + L" turns left before program clears");
	fontSprite->Draw(camera, deltaTime);
	fontSprite->End();
	energyIcon->SetPosition(backButton->GetWorldX(), backButton->GetWorldY() - 36.f);
	energyIcon->Begin();
	energyIcon->Draw(camera, deltaTime);
	energyIcon->End();
	hourglassIcon->Begin();
	hourglassIcon->SetPosition(executeButton->GetWorldX(), executeButton->GetWorldY() - 36.f);
	hourglassIcon->Draw(camera, deltaTime);
	hourglassIcon->End();
	if (!mainBlockCard->IsExecuting() && !isTransitioning) {
		backButton->Draw(game->GetGraphicsDevice(), deltaTime);
		executeButton->Draw(game->GetGraphicsDevice(), deltaTime);
	}
	draggableManager->Draw(deltaTime);
}

void Demo::IBattleScene::PlayerOpenItemsDraw(unsigned long long deltaTime)
{
	itemMenuBackground->Begin();
	itemMenuBackground->Draw(camera, deltaTime);
	itemMenuBackground->End();

	closeItemMenuButton->Draw(game->GetGraphicsDevice(), deltaTime);

	for (auto& btn : buffItems) 
	{
		btn->Draw(game->GetGraphicsDevice(), deltaTime);
	}

	auto& inventory = player->GetInventoryItems().GetSlots();

	fontSprite->Begin();
	int displayIndex = 0;
	for (int i = 0; i < inventory.size(); i++) {
		if (inventory[i].quantity <= 0) continue;

		auto btn = buffItems[displayIndex];
		float textX = btn->GetWorldX()+ 15.0f;
		float textY = btn->GetWorldY() + 70.0f;

		fontSprite->SetPosition(textX, textY);

		//draw item's quantity below iconbutton
		fontSprite->SetText(L"x" + std::to_wstring(inventory[i].quantity));
		fontSprite->Draw(camera, deltaTime);

		displayIndex++;
	}
	fontSprite->End();

}

void Demo::IBattleScene::EnemyAttackDraw(unsigned long long deltaTime)
{
	battlePlayer->Draw(deltaTime);
	auto app = DX9GF::Application::GetInstance();
	auto gd = game->GetGraphicsDevice();
	const float spacing = 5.f;
	const float w = battlePlayer->GetMaxHealth() * spacing;
	const float w_ = battlePlayer->GetHealth() * spacing;
	const float x = -w / 2;
	const float y = app->GetScreenHeight() / 2.f - 40;
	gd->DrawRectangle(camera, x, y, w, 20, 0xFFFF0000, true);
	gd->DrawRectangle(camera, x, y, w_, 20, 0xFF00FF00, true);
	gd->DrawRectangle(camera, x, y, w, 20, 0xFF000000, false);
}

void Demo::IBattleScene::Init()
{
	// Init components
	draggableManager = std::make_shared<DraggableManager>();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"StatusPlz", 16);
	ItemData::GetInstance()->LoadData();
	player->GiveTestItems();
	// Setup player
	battlePlayer = std::make_shared<Player>(transformManager);
	battlePlayer->Init(game->GetGraphicsDevice(), &colliderManager, &camera);
	battlePlayer->SetFollowCamera(false);
	battlePlayer->SetVelocity(125);
	battlePlayer->SetHealth(player->GetHealth());

	// Initialize texture sheet
	uiSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	uiSheetTex->LoadTexture(L"ui.png");
	tempTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	tempTex->LoadTexture(L"TempTex.png");
	// Create buttons
	const auto buttonWidth = 96;
	const auto buttonHeight = 32;
	attackButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	attackButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerAttack;
			for (auto& enemy : enemies) {
				enemy->SetState(false);
			}
			markFinished();
			}));
		isExecutingAttacks = false;
		});
	attackButton->SetSpriteRects({
		{
			.left = 0,
			.top = 48,
			.right = 48,
			.bottom = 64
		},
		{
			.left = 0,
			.top = 64,
			.right = 48,
			.bottom = 80
		},
		{
			.left = 0,
			.top = 80,
			.right = 48,
			.bottom = 96
		}
		});
	attackButton->SetSpriteScale(2.f, 2.f);
	actionButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	actionButton->SetSpriteRects({
		{
			.left = 48,
			.top = 48,
			.right = 96,
			.bottom = 64
		},
		{
			.left = 48,
			.top = 64,
			.right = 96,
			.bottom = 80
		},
		{
			.left = 48,
			.top = 80,
			.right = 96,
			.bottom = 96
		}
		});
	actionButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		});
	actionButton->SetSpriteScale(2.f, 2.f);

	itemsButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	itemsButton->SetSpriteRects({
		{
			.left = 0,
			.top = 96,
			.right = 48,
			.bottom = 112
		},
		{
			.left = 0,
			.top = 112,
			.right = 48,
			.bottom = 128
		},
		{
			.left = 0,
			.top = 128,
			.right = 48,
			.bottom = 144
		}
		});
	itemsButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->RefreshItemMenu();
			this->state = State::PlayerOpenItems;
			//turn off volume or play sfx volume when open a menu here
			markFinished();
			}));
		});
	itemsButton->SetSpriteScale(2.f, 2.f);
	fleeButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	fleeButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this](std::function<void(void)> markFinished) {
			player->SetHealth(battlePlayer->GetHealth());
			auto sceMan = game->GetSceneManager();
			sceMan->PopScene();
			sceMan->GoToPrevious();
			markFinished();
			}));
		});
	fleeButton->SetSpriteRects({
		{
			.left = 48,
			.top = 96,
			.right = 96,
			.bottom = 112
		},
		{
			.left = 48,
			.top = 112,
			.right = 96,
			.bottom = 128
		},
		{
			.left = 48,
			.top = 128,
			.right = 96,
			.bottom = 144
		}
		});
	fleeButton->SetSpriteScale(2.f, 2.f);
	backButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	backButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerStandBy;
			for (auto& enemy : enemies) {
				enemy->SetState(true);
			}
			markFinished();
			}));
		});
	backButton->SetSpriteRects({
		{
			.left = 96,
			.top = 48,
			.right = 144,
			.bottom = 64
		},
		{
			.left = 96,
			.top = 64,
			.right = 144,
			.bottom = 80
		},
		{
			.left = 96,
			.top = 80,
			.right = 144,
			.bottom = 96
		}
		});
	backButton->SetSpriteScale(2.f, 2.f);
	executeButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	executeButton->SetSpriteRects({
		{
			.left = 96,
			.top = 96,
			.right = 144,
			.bottom = 112
		},
		{
			.left = 96,
			.top = 112,
			.right = 144,
			.bottom = 128
		},
		{
			.left = 96,
			.top = 128,
			.right = 144,
			.bottom = 144
		}
		});
	executeButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		if (usedEnergy > energy) {
			popUpMessage->QueueMessage(&commandBuffer, L"Not enough energy");
		}
		else if (mainBlockCard && !mainBlockCard->IsExecuting()) {
			isExecutingAttacks = true;
			mainBlockCard->StartExecution();
		}
		});
	executeButton->SetSpriteScale(2.f, 2.f);

	//temporary copy backbutton texture
	closeItemMenuButton = std::make_shared<IconButton>(transformManager, 0, 0, buttonWidth, buttonHeight, uiSheetTex);
	closeItemMenuButton->SetOnReleaseLeft([&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerStandBy;
			markFinished();
			}));
		});
	closeItemMenuButton->SetSpriteRects({
		{
			.left = 96,
			.top = 48,
			.right = 144,
			.bottom = 64
		},
		{
			.left = 96,
			.top = 64,
			.right = 144,
			.bottom = 80
		},
		{
			.left = 96,
			.top = 80,
			.right = 144,
			.bottom = 96
		}
		});
	closeItemMenuButton->SetSpriteScale(2.f, 2.f);

	// Init buttons
	attackButton->Init(&camera);
	actionButton->Init(&camera);
	itemsButton->Init(&camera);
	fleeButton->Init(&camera);
	backButton->Init(&camera);
	executeButton->Init(&camera);
	closeItemMenuButton->Init(&camera);

	// Init sprite
	fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	fontSprite->SetColor(0xFF000000);
	energyIcon = std::make_shared<DX9GF::StaticSprite>(uiSheetTex.get());
	energyIcon->SetSrcRect({
		.left = 96,
		.top = 0,
		.right = 112,
		.bottom = 16
		});
	energyIcon->SetScale(2.f);
	hourglassIcon = std::make_shared<DX9GF::StaticSprite>(uiSheetTex.get());
	hourglassIcon->SetSrcRect({
		.left = 112,
		.top = 0,
		.right = 128,
		.bottom = 16
		});
	hourglassIcon->SetScale(2.f);


	itemMenuBackground = std::make_shared<DX9GF::StaticSprite>(tempTex.get());
	itemMenuBackground->SetSrcRect({
		.left = 312,
		.top = 0,
		.right = 504,
		.bottom = 128 });
	itemMenuBackground->SetOrigin(95.5f, 63.5f);
	itemMenuBackground->SetScale(3.0f, 3.0f);

	// Init draggables
	mainBlockCard = std::make_shared<MainBlockCard>(transformManager, -100.f, -140.f);
	mainBlockCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	handContainer = std::make_shared<HandContainer>(transformManager, 180, 40, -250.f, -200.f);
	handContainer->Init(draggableManager, game->GetGraphicsDevice(), &camera, &playedPile);

	// Init pop up message
	popUpMessage = std::make_shared<PopUpMessage>(transformManager);
	popUpMessage->Init(game->GetGraphicsDevice(), &camera);

	transformManager->RebuildHierarchy();
	DamageTextManager::GetInstance()->Init(this->game);
}

void Demo::IBattleScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
	if (!enemyLayoutInitialized || lastEnemyLayoutState != state) {
		if (state == State::PlayerStandBy || state == State::PlayerAttack) {
			QueueEnemyLayoutTransition(state);
			lastEnemyLayoutState = state;
			enemyLayoutInitialized = true;
		}
	}
	switch (state) {
	case State::PlayerStandBy:
		PlayerStandByUpdate(deltaTime);
		break;
	case State::PlayerAttack:
		PlayerAttackUpdate(deltaTime);
		break;
	case State::PlayerOpenItems:
		PlayerOpenItemsUpdate(deltaTime);
		break;
	case State::EnemyAttack:
		EnemyAttackUpdate(deltaTime);
		break;
	default:
		throw std::runtime_error("Unexpected state");
	}
	DamageTextManager::GetInstance()->Update(deltaTime);
	transformManager->UpdateAll();
	commandBuffer.Update(deltaTime);
}

void Demo::IBattleScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	auto inpMan = DX9GF::InputManager::GetInstance();
	gd->Clear(0xFFFFFFFF);
	if (SUCCEEDED(gd->BeginDraw())) {
		for (auto& enemy : enemies) {
			enemy->Draw(game->GetGraphicsDevice(), &camera, deltaTime);
		}
		switch (state) {
		case State::PlayerStandBy:
			PlayerStandByDraw(deltaTime);
			break;
		case State::PlayerAttack:
			PlayerAttackDraw(deltaTime);
			break;
		case State::PlayerOpenItems:
			PlayerOpenItemsDraw(deltaTime);
			break;
		case State::EnemyAttack:
			EnemyAttackDraw(deltaTime);
			break;
		default:
			throw std::runtime_error("Unexpected state");
		}
		for (auto& card : queuedToDraw) {
			dynamic_pointer_cast<IDraggable>(card)->Draw(deltaTime);
		}
		popUpMessage->Draw(deltaTime);
		DamageTextManager::GetInstance()->Draw(this->camera, deltaTime);
		inpMan->DrawCursor(&camera, deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}
