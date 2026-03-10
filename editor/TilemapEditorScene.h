#pragma once
#include "DX9GF.h"
#include "Tileset.h"
#include <memory>

namespace Editor {
	class TilemapEditorScene : public DX9GF::IScene {
		DX9GF::IGame* editor;
		bool showGrid = true;
		int gridSpacingX = 64;
		int gridSpacingY = 64;
		float gridColorR = 1;
		float gridColorG = 1;
		float gridColorB = 1;
		float gridColorA = 0.5f;
		std::shared_ptr<DX9GF::StaticSprite> spritesheet;
		std::shared_ptr<Tileset> tileset;
	public:
		TilemapEditorScene(DX9GF::IGame* editor, int screenWidth, int screenHeight) : IScene(screenWidth, screenHeight), editor(editor) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}