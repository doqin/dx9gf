#include "pch.h"
#include "TestBattleScene.h"

void Demo::TestBattleScene::Init()
{
	IBattleScene::Init();
	auto testEnemy = std::make_shared<TestEnemy>(transformManager, 20.f);
	testEnemy->Init(game->GetGraphicsDevice(), &camera);
	testEnemy->SetOnRequestEnemyCard([this](std::shared_ptr<IEnemy> enemy) {
		CreateEnemyCard(enemy);
	});
	enemies.push_back(testEnemy);

	cardDeck.push_back(std::make_shared<StrikeCard>(transformManager, -260.f, -80.f));
	cardDeck.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	cardDeck.push_back(std::make_shared<StrikeCard>(transformManager, -260.f, -80.f));
	cardDeck.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	cardDeck.push_back(std::make_shared<StrikeCard>(transformManager, -260.f, -80.f));
	cardDeck.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	transformManager->RebuildHierarchy();
}

void Demo::TestBattleScene::EnemyAttackUpdate(unsigned long long deltaTime)
{
}
