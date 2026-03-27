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

		//volume UI
		std::shared_ptr<DX9GF::StaticSprite> trackMaster, trackMusic, trackSFX;
		std::shared_ptr<Demo::IconButton> knobMaster, knobMusic, knobSFX;
		// Biến kiểm soát trạng thái kéo chuột
		bool isDraggingMaster = false;
		bool isDraggingMusic = false;
		bool isDraggingSFX = false;
		// --- KEYBIND UI ---
		// 4 nút cho 4 hướng di chuyển
		std::shared_ptr<Demo::IconButton> btnUp, btnDown, btnLeft, btnRight;

		// 4 cờ hiệu để biết đang "đợi" phím nào
		bool isListeningUp = false;
		bool isListeningDown = false;
		bool isListeningLeft = false;
		bool isListeningRight = false;

	public:
		SettingsScene(Game* game, int screenWidth, int screenHeight)
			: IScene(screenWidth, screenHeight),
			game(game) {
		}

		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
		void UpdateLayout(int width, int height);
		void DrawString(std::wstring text, float x, float y, D3DCOLOR color);
		void ResetListening();
	};
}

