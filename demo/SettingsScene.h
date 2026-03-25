#pragma once
#include "Game.h"
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "IconButton.h"
namespace Demo
{
	class SettingsScene : public DX9GF::IScene
	{
	private:
		Game* game;
		std::shared_ptr<DX9GF::TransformManager> transformManager;

		std::shared_ptr<DX9GF::StaticSprite> bgSprite;
		std::shared_ptr<DX9GF::StaticSprite> titleSprite;
		std::shared_ptr<DX9GF::Texture> bgTex;
		std::shared_ptr<DX9GF::Texture> titleTex;

		//button component
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<DX9GF::Texture> buttonSheetTex;
		std::vector<std::shared_ptr<Demo::IButton>> uiButtons;

		std::shared_ptr<IconButton> backButton;

		bool isGoingBack = false; // Cờ báo hiệu muốn chuyển Scene

		//use to scale or relocate the sprite/object
		int lastScreenWidth;
		int lastScreenHeight;
	public:
		SettingsScene(Game* game, int screenWidth, int screenHeight)
			: IScene(screenWidth, screenHeight),
			game(game){}

		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
		void UpdateLayout(int width, int height);
	};
}

