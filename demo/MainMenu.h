#pragma once
#include "DX9GF.h"
#include "DX9GFIScene.h"
#include "Game.h"
#include "IButton.h"
#include "DX9GFExtras.h"
#include <vector>
#include <memory>
namespace Demo
{
	class MainMenu : public DX9GF::IScene
	{
	private:
		Game* game;

		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<DX9GF::StaticSprite> backgroundSprite;
		std::shared_ptr<DX9GF::StaticSprite> titleSprite;

		std::shared_ptr<DX9GF::Texture> backgroundTex;
		std::shared_ptr<DX9GF::Texture> titleTex;

		//UI component
		std::vector<std::shared_ptr<IButton>> uiButtons;
		std::shared_ptr<DX9GF::Texture> uiSheetTex;
		//std::shared_ptr<DX9GF::Font> myFont;


	public:

		MainMenu(Game* game, int screenWidth, int screenHeight)
			: IScene(screenWidth, screenHeight),
			game(game)
		{
		}		
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}

