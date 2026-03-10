#pragma once
#include "DX9GF.h"
#include "Tileset.h"
#include <memory>

namespace Editor {
	class TilesetEditorScene : public DX9GF::IScene {
		DX9GF::IGame* editor;
		std::weak_ptr<Tileset> tileset;
	public:
		TilesetEditorScene(
			DX9GF::IGame* editor, 
			std::weak_ptr<Tileset> tileset, 
			int screenWidth, 
			int screenHeight
		) : IScene(screenWidth, screenHeight), editor(editor), tileset(tileset) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}