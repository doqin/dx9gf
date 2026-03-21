#include "pch.h"
#include "IBattleScene.h"

void Demo::IBattleScene::PlayerStandByUpdate(unsigned long long deltaTime)
{
	auto app = DX9GF::Application::GetInstance();
	// Set position so that it's updated if screen is resized
	const float screenWidth = app->GetScreenWidth();
	const float buttonY = app->GetScreenHeight() / 2.f - 20 - attackButton->GetHeight();
	const float sidePadding = 20.f;
	const float totalButtonsWidth = attackButton->GetWidth() + actionButton->GetWidth() + itemsButton->GetWidth() + fleeButton->GetWidth();
	const float spacing = std::max(0.f, (screenWidth - sidePadding * 2.f - totalButtonsWidth) / 3.f);
	const float leftX = -screenWidth / 2.f + sidePadding;

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
}

void Demo::IBattleScene::EnemyAttackDraw(unsigned long long deltaTime)
{
}

void Demo::IBattleScene::Init()
{
	transformManager = std::make_shared<DX9GF::TransformManager>();
	font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 20);
	attackButton = std::make_shared<TextButton>(transformManager, 0, 0, 100, 25, "Attack", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			this->state = State::PlayerAttack;
		}));
	});
	actionButton = std::make_shared<TextButton>(transformManager, 0, 0, 100, 25, "Action", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	itemsButton = std::make_shared<TextButton>(transformManager, 0, 0, 100, 25, "Items", font.get(), [](DX9GF::ITrigger* thisObj) {
	});
	fleeButton = std::make_shared<TextButton>(transformManager, 0, 0, 100, 25, "Flee", font.get(), [&](DX9GF::ITrigger* thisObj) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([&](std::function<void(void)> markFinished) {
			auto sceMan = game->GetSceneManager();
			sceMan->PopScene();
			sceMan->GoToPrevious();
			markFinished();
		}));
	});
	attackButton->Init(&camera);
	actionButton->Init(&camera);
	itemsButton->Init(&camera);
	fleeButton->Init(&camera);
	transformManager->RebuildHierarchy();
}

void Demo::IBattleScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
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
