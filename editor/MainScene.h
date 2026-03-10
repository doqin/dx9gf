#pragma once
#include "DX9GF.h"

namespace Editor {
	class MainScene : public DX9GF::IScene {
		DX9GF::IGame* editor;
		bool showGrid = true;
		int gridSpacingX = 64;
		int gridSpacingY = 64;
		float gridColorR = 1;
		float gridColorG = 1;
		float gridColorB = 1;
		float gridColorA = 0.5f;
	public:
		MainScene(DX9GF::IGame* editor, int screenWidth, int screenHeight) : IScene(screenWidth, screenHeight), editor(editor) {}
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}