#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "Player.h"

namespace Demo {
	class TutorialWorldScene : public DX9GF::IScene {
		Game* game;
		std::shared_ptr<Player> player;
		std::shared_ptr<DX9GF::ColliderManager> colliderManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<DX9GF::Map> map;
	public:
		TutorialWorldScene(Game* game, UINT sw, UINT sh) : IScene(sw, sh), game(game) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}