#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "Player.h"
#include "TextButton.h"
#include "IContainer.h"

namespace Demo {
	class InventoryMenu {
	public:
		enum class Tab { ITEMS, DECK };

	private:
		Game* game;
		std::shared_ptr<Player> player;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<DraggableManager> draggableManager;
		DX9GF::Camera* uiCamera;

		DX9GF::Font* font;
		bool isOpen = false;
		Tab currentTab = Tab::ITEMS;

		// UI Buttons
		std::shared_ptr<TextButton> btnTabItems;
		std::shared_ptr<TextButton> btnTabDeck;
		std::shared_ptr<TextButton> btnResume;
		std::shared_ptr<TextButton> btnOptions;
		std::shared_ptr<TextButton> btnLeaveGame;

		std::shared_ptr<DX9GF::FontSprite> fontSprite;

		// Deck Builder Containers
		std::shared_ptr<IContainer> deckContainer;
		std::shared_ptr<IContainer> inventoryContainer;

		bool pendingLeave = false;

	public:
		InventoryMenu(Game* g, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::TransformManager> tm, std::shared_ptr<DraggableManager> dm, DX9GF::Camera* cam, DX9GF::Font* font);

		void Init();
		void Update(unsigned long long deltaTime);
		void Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime);

		void Toggle();
		bool IsOpen() const { return isOpen; }
		void SetTab(Tab tab) { currentTab = tab; }
		bool IsPendingLeave() const { return pendingLeave; }
		Tab GetCurrentTab() const { return currentTab; }
	};
}