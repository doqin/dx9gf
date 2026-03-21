#pragma once
#include "IBattleScene.h"

namespace Demo {
	class TestBattleScene : public IBattleScene {
	public:
		TestBattleScene(Game* game, std::shared_ptr<Player> player, int screenWidth, int screenHeight) : IBattleScene(game, player, screenWidth, screenHeight) {}
		void EnemyAttackUpdate(unsigned long long deltaTime) override;
	};
}