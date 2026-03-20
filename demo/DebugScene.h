#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "IDraggable.h"
#include "IContainer.h"

#include "DX9GFFont.h"
#include "TextButton.h"
#include "IconButton.h"

namespace Demo {
	class DebugScene : public DX9GF::IScene {
		Game* game;
		std::shared_ptr<DraggableManager> draggableManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::vector<std::shared_ptr<IContainer>> containers;
		std::vector<std::shared_ptr<IDraggable>> draggables;

		//UI component
		std::vector<std::shared_ptr<Demo::IButton>> uiButtons;
		std::shared_ptr<DX9GF::Texture> uiSheetTex;
		std::shared_ptr<DX9GF::Font> myFont;

	public:
		DebugScene(Game* game, int screenWidth, int screenHeight) : IScene(screenWidth, screenHeight), game(game) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}