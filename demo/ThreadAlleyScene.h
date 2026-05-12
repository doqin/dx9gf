#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "Player.h"
#include "SavePoint.h"
#include "InventoryMenu.h"
#include "StrikeCard.h"
#include "ShopPoint.h"
#include "HealingPoint.h"

namespace Demo {
	class ThreadAlleyScene : public DX9GF::IScene, public DX9GF::ISaveable {
		bool isGamePaused = false;
		Game* game;
		DX9GF::Camera uiCamera;
		std::shared_ptr<DX9GF::ColliderManager> colliderManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<Demo::DraggableManager> draggableManager;
		std::shared_ptr<InventoryMenu> inventoryMenu;
		std::shared_ptr<DX9GF::SaveManager> saveManager;

		std::vector<std::shared_ptr<SavePoint>> savePoints;
		std::vector<std::shared_ptr<ShopPoint>> shopPoints;
		std::vector<std::shared_ptr<HealingPoint>> healingPoints;

		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<Player> player;
		std::shared_ptr<DX9GF::Map> map;
		std::shared_ptr<DX9GF::CommandBuffer> drawBuffer;
		std::shared_ptr<DX9GF::CommandBuffer> commandBuffer;

	public:
		ThreadAlleyScene(Game* game, std::shared_ptr<DX9GF::SaveManager> sm, UINT sw, UINT sh) : IScene(sw, sh), game(game), saveManager(sm), uiCamera(sw, sh) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;

		std::string GetSaveID() const override;
		void GenerateSaveData(nlohmann::json& outData) override;
		void RestoreSaveData(const nlohmann::json& inData) override;

		void GiveTestItems();
	};
}