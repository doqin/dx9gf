#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "Player.h"
#include "SavePoint.h"

namespace Demo {
	class TutorialWorldScene : public DX9GF::IScene {
		Game* game;
		std::shared_ptr<DX9GF::ColliderManager> colliderManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<DX9GF::SaveManager> saveManager;

		std::shared_ptr<SavePoint> savePoint;
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<Player> player;
		std::shared_ptr<DX9GF::Map> map;

		bool shouldLoadSave = false;
	public:
		TutorialWorldScene(Game* game, UINT sw, UINT sh) : IScene(sw, sh), game(game) {}

		void SetLoadSave(bool value) { shouldLoadSave = value; }
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}