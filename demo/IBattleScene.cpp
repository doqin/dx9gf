#include "pch.h"
#include "IBattleScene.h"
#include <algorithm>
#include <random>

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
			MoveExecutedHandCardsToPlayedPile();
			MoveHandCardsToDiscardPile();
			// Remove unused enemy cards or enemy cards of dead enemies
			for (size_t i = 0; i < enemyCards.size(); ++i) {
				auto& enemyCard = enemyCards[i];
				if (!enemyCard->GetParent().has_value() || enemyCard->GetValue()->IsDead()) {
					if (auto manager = enemyCard->GetDraggableManager().lock()) {
						manager->Remove(enemyCard);
					}
					enemyCards.erase(enemyCards.begin() + i);
					--i;
				}
			}
			// Remove dead enemies
			for (size_t i = 0; i < enemies.size(); ++i) {
				if (enemies[i]->IsDead()) {
					enemies.erase(enemies.begin() + i);
					--i;
				}
			}
			state = State::EnemyAttack;
			QueueEnemyLayoutTransition(State::EnemyAttack);
			for (auto& enemy : enemies) {
				enemy->SetState(true);
			}
			for (auto& enemy : enemies) {
				if (enemy->HasStatus(StatusType::STUN)) {
					continue;
				}
				enemy->StartAttack(battlePlayer);
			}
			battlePlayer->SetLocalPosition(0, 0);
			EnemyAttackUpdate(deltaTime);
			return;
		}
		backButton->Update(deltaTime);
		executeButton->Update(deltaTime);
	}
	for (auto& enemy : enemies) {
		enemy->Update(deltaTime);
	}
	draggableManager->Update(deltaTime);
	RemoveEnemyCardsInRemoveArea();
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
		for (auto& enemy : enemies) {
			enemy->TickStatuses();
		}
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
	if (font) {
		fontSprite->Begin();
		fontSprite->SetPosition(enemyCardRemoveAreaX + 8.f, enemyCardRemoveAreaY + 8.f);
		fontSprite->SetText(L"Drop EnemyCard Here");
		fontSprite->Draw(camera, deltaTime);
		fontSprite->SetPosition(backButton->GetWorldX(), backButton->GetWorldY() - 30.f);
		fontSprite->SetText(std::to_wstring(energy - usedEnergy) + L"/" + std::to_wstring(MAX_ENERGY));
		fontSprite->Draw(camera, deltaTime);
		fontSprite->SetPosition(executeButton->GetWorldX(), executeButton->GetWorldY() - 30.f);
		fontSprite->SetText(std::to_wstring(3 - (currentTurn - 1) % 3) + L" turns left before program clears");
		fontSprite->Draw(camera, deltaTime);
		fontSprite->End();
	}
	if (!mainBlockCard->IsExecuting()) {
		backButton->Draw(game->GetGraphicsDevice(), deltaTime);
		executeButton->Draw(game->GetGraphicsDevice(), deltaTime);
	}
	draggableManager->Draw(deltaTime);
}

void Demo::IBattleScene::EnemyAttackDraw(unsigned long long deltaTime)
{
	battlePlayer->Draw(deltaTime);
	auto app = DX9GF::Application::GetInstance();
	auto gd = game->GetGraphicsDevice();
	gd->DrawRectangle(camera, -battlePlayer->GetMaxHealth() / 2, app->GetScreenHeight() / 2.f - 40, battlePlayer->GetMaxHealth(), 20, 0xFFFF0000, true);
	gd->DrawRectangle(camera, -battlePlayer->GetMaxHealth() / 2, app->GetScreenHeight() / 2.f - 40, battlePlayer->GetHealth(), 20, 0xFF00FF00, true);
}

void Demo::IBattleScene::Init()
{
	// Init components
	draggableManager = std::make_shared<DraggableManager>();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 20);
	// Setup player
	battlePlayer = std::make_shared<Player>(transformManager);
	battlePlayer->Init(game->GetGraphicsDevice(), &colliderManager, &camera);
	battlePlayer->SetFollowCamera(false);
	battlePlayer->SetVelocity(125);
	battlePlayer->SetHealth(player->GetHealth());

	// Create buttons
	const auto buttonWidth = 100;
	const auto buttonHeight = 50;
	attackButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Attack", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerAttack;
			for (auto& enemy : enemies) {
				enemy->SetState(false);
			}
			markFinished();
		}));
		isExecutingAttacks = false;
	});
	actionButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Action", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	itemsButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Items", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	fleeButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Flee", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this](std::function<void(void)> markFinished) {
			player->SetHealth(battlePlayer->GetHealth());
			auto sceMan = game->GetSceneManager();
			sceMan->PopScene();
			sceMan->GoToPrevious();
			markFinished();
		}));
	});
	backButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Back", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerStandBy;
			for (auto& enemy : enemies) {
				enemy->SetState(true);
			}
			markFinished();
		}));
	});
    executeButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Execute", font.get(), [&](DX9GF::ITrigger* thisObj) {
		if (usedEnergy > energy) {
			popUpMessage->QueueMessage(&commandBuffer, L"Not enough energy");
		}
		else if (mainBlockCard && !mainBlockCard->IsExecuting()) {
			isExecutingAttacks = true;
			mainBlockCard->StartExecution();
		}
	});
	// Init buttons
	attackButton->Init(&camera);
	actionButton->Init(&camera);
	itemsButton->Init(&camera);
	fleeButton->Init(&camera);
	backButton->Init(&camera);
	executeButton->Init(&camera);

	// Init sprite
	fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());

	// Init draggables
	mainBlockCard = std::make_shared<MainBlockCard>(transformManager, -100.f, -140.f);
	mainBlockCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	handContainer = std::make_shared<HandContainer>(transformManager, 180, 40, -250.f, -200.f);
	handContainer->Init(draggableManager, game->GetGraphicsDevice(), &camera, &playedPile);

	// Init pop up message
	popUpMessage = std::make_shared<PopUpMessage>(transformManager);
	popUpMessage->Init(game->GetGraphicsDevice(), &camera);

	transformManager->RebuildHierarchy();
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
	case State::EnemyAttack:
		EnemyAttackUpdate(deltaTime);
		break;
	default:
		throw std::runtime_error("Unexpected state");
	}
	transformManager->UpdateAll();
	commandBuffer.Update(deltaTime);
}

void Demo::IBattleScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear();
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
		gd->EndDraw();
	}
	gd->Present();
}
