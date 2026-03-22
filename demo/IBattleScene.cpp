#include "pch.h"
#include "IBattleScene.h"
#include <algorithm>

void Demo::IBattleScene::QueueEnemyLayoutTransition(State targetState)
{
	if (enemies.empty()) {
		return;
	}

	const auto app = DX9GF::Application::GetInstance();
	const float moveVelocity = 1000.f;
	const float centerLineY = -120.f;
	const float horizontalSpacing = 120.f;
	const float verticalSpacing = 100.f;
	const float rightSideX = app->GetScreenWidth() / 2.f - 120.f;

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
		else {
			const float totalHeight = (enemyCount > 1) ? (enemyCount - 1) * verticalSpacing : 0.f;
			targetX = rightSideX;
			targetY = -totalHeight / 2.f + i * verticalSpacing;
		}

		auto command = std::make_shared<DX9GF::GoToCommand>(enemies[i], targetX, targetY, moveVelocity);
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
	auto card = std::make_shared<EnemyCard>(transformManager, enemy, -260.f, -140.f);
	card->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	cardDeck.push_back(card);
}

void Demo::IBattleScene::RemoveEnemyCardsInRemoveArea()
{
	const float left = enemyCardRemoveAreaX;
	const float top = enemyCardRemoveAreaY;
	const float right = enemyCardRemoveAreaX + enemyCardRemoveAreaWidth;
	const float bottom = enemyCardRemoveAreaY + enemyCardRemoveAreaHeight;

	cardDeck.erase(std::remove_if(cardDeck.begin(), cardDeck.end(), [&](const std::shared_ptr<IDraggable>& card) {
		auto enemyCard = std::dynamic_pointer_cast<EnemyCard>(card);
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
	}), cardDeck.end());
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
}

void Demo::IBattleScene::PlayerAttackUpdate(unsigned long long deltaTime)
{
	auto app = DX9GF::Application::GetInstance();
	const float screenWidth = app->GetScreenWidth();
	const float buttonY = app->GetScreenHeight() / 2.f - 20 - attackButton->GetHeight();
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
		if (isExecutingAttacks) {
			state = State::EnemyAttack;
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
	player->Update(deltaTime);
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
		auto removeLabel = std::make_shared<DX9GF::FontSprite>(font.get());
		removeLabel->Begin();
		removeLabel->SetPosition(enemyCardRemoveAreaX + 8.f, enemyCardRemoveAreaY + 8.f);
		removeLabel->SetText(L"Drop EnemyCard Here");
		removeLabel->Draw(camera, deltaTime);
		removeLabel->End();
	}
	if (!mainBlockCard->IsExecuting()) {
		backButton->Draw(game->GetGraphicsDevice(), deltaTime);
		executeButton->Draw(game->GetGraphicsDevice(), deltaTime);
	}
	draggableManager->Draw(deltaTime);
}

void Demo::IBattleScene::EnemyAttackDraw(unsigned long long deltaTime)
{
	player->Draw(deltaTime);
}

void Demo::IBattleScene::Init()
{
	// Init components
	draggableManager = std::make_shared<DraggableManager>();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 20);
	// Setup player
	player->SetFollowCamera(false);
	previousPlayerTransformHandle = player->GetTransformHandle();
	previousSceneTransformManager = player->GetTransformManager();
	previousPlayerVelocity = player->GetVelocity();
	player->SetTransformManager(transformManager);
	player->SetTransformHandle(player->CreateTransform());
	player->SetVelocity(125);

	// Create buttons
	const auto buttonWidth = 100;
	const auto buttonHeight = 50;
	attackButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Attack", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerAttack;
			markFinished();
		}));
		isExecutingAttacks = false;
	});
	actionButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Action", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	itemsButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Items", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	fleeButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Flee", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			auto sceMan = game->GetSceneManager();
			sceMan->PopScene();
			sceMan->GoToPrevious();
			player->SetFollowCamera(true);
			player->SetTransformHandle(previousPlayerTransformHandle);
			player->SetTransformManager(previousSceneTransformManager);
			player->SetVelocity(previousPlayerVelocity);
			markFinished();
		}));
	});
	backButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Back", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerStandBy;
			markFinished();
		}));
	});
    executeButton = std::make_shared<TextButton>(transformManager, 0, 0, buttonWidth, buttonHeight, "Execute", font.get(), [&](DX9GF::ITrigger* thisObj) {
		if (mainBlockCard && !mainBlockCard->IsExecuting()) {
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

	mainBlockCard = std::make_shared<MainBlockCard>(transformManager, -100.f, -140.f);
	mainBlockCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
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
	for (auto& enemy : enemies) {
		enemy->Draw(game->GetGraphicsDevice(), &camera, deltaTime);
	}
	if (SUCCEEDED(gd->BeginDraw())) {
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
		gd->EndDraw();
	}
	gd->Present();
}
