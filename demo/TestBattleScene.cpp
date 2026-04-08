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

	/*for (size_t i = 0; i < 9; ++i) {
		auto strikeCard = std::make_shared<StrikeCard>(transformManager, -260.f, -80.f);
		strikeCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
		drawPile.push_back(strikeCard);
	}*/

	// Heavy Strike
	auto heavy = std::make_shared<HeavyStrikeCard>(transformManager, -260.f, -80.f);
	heavy->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(heavy);

	// Twin Strike
	auto twin = std::make_shared<TwinStrikeCard>(transformManager, -260.f, -80.f);
	twin->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(twin);

	// Cleave
	auto cleave = std::make_shared<CleaveCard>(transformManager, -260.f, -80.f);
	cleave->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(cleave);

	// Chain Lightning
	auto chain = std::make_shared<ChainLightningCard>(transformManager, -260.f, -80.f);
	chain->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(chain);

	// Poison
	auto poison = std::make_shared<PoisonCard>(transformManager, -260.f, -80.f);
	poison->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(poison);

	// Vulnerable
	auto vuln = std::make_shared<VulnerableCard>(transformManager, -260.f, -80.f);
	vuln->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(vuln);

	// Weakness
	auto weak = std::make_shared<WeaknessCard>(transformManager, -260.f, -80.f);
	weak->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(weak);

	// Stun
	auto stun = std::make_shared<StunCard>(transformManager, -260.f, -80.f);
	stun->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(stun);

	// Strike
	auto strikeCard = std::make_shared<StrikeCard>(transformManager, -260.f, -80.f);
	strikeCard->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	drawPile.push_back(strikeCard);

	StartBattle();
	transformManager->RebuildHierarchy();
}
