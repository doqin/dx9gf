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
	testEnemy = std::make_shared<TestEnemy>(transformManager, 20.f);
	testEnemy->Init(game->GetGraphicsDevice(), &camera);
	testEnemy->SetOnRequestEnemyCard([this](std::shared_ptr<IEnemy> enemy) {
		CreateEnemyCard(enemy);
	});
	enemies.push_back(testEnemy);

	for (size_t i = 0; i < 9; ++i) {
		auto strikeCard = std::make_shared<StrikeCard>(transformManager, -260.f, -80.f);
		strikeCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
		drawPile.push_back(strikeCard);
	}
	StartBattle();
	transformManager->RebuildHierarchy();
}
