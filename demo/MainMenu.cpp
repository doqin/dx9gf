#include "pch.h"
#include "MainMenu.h"
#include "resource.h"
#include "IconButton.h"
#include "SettingsScene.h"
#include "TutorialWorldScene.h"

namespace Demo
{
	void MainMenu::UpdateLayout(int screenW, int screenH)
	{
		//BACKGROUND - use aspect fill
		float bgImageW = (float)bgTex->GetWidth();
		float bgImageH = (float)bgTex->GetHeight();

		if (!bgSprite)
		{
			bgSprite = std::make_shared<DX9GF::StaticSprite>(bgTex.get());
			bgSprite->SetSrcRect({ 0, 0, (LONG)bgImageW, (LONG)bgImageH });
		}

		float bgScaleX = screenW / bgImageW;
		float bgScaleY = screenH / bgImageH;
		float bgFinalScale = std::max(bgScaleX, bgScaleY);

		bgSprite->SetScale(bgFinalScale);
		bgSprite->SetOrigin(bgImageW / 2.0f, bgImageH / 2.0f);
		bgSprite->SetPosition(0, 0);

		//TITLE - use scale to fit (have a limit)
		float titleImageW = (float)titleTex->GetWidth();
		float titleImageH = (float)titleTex->GetHeight();

		if (!titleSprite) {
			titleSprite = std::make_shared<DX9GF::StaticSprite>(titleTex.get());
			titleSprite->SetSrcRect({ 0, 0, (LONG)titleImageW, (LONG)titleImageH });
		}

		float maxTitleWidth = screenW * 0.5f;
		float titleScale = maxTitleWidth / titleImageW;
		float maxTitleHeight = screenH * 0.4f;

		if ((titleImageH * titleScale) > maxTitleHeight) {
			titleScale = maxTitleHeight / titleImageH;
		}

		titleSprite->SetScale(titleScale);
		titleSprite->SetOrigin(titleImageW / 2.0f, titleImageH / 2.0f);
		titleSprite->SetPosition(0, -screenH * 0.20f);

		//spacing between buttons
		float spacingY = 10.0f;
		//start drawing from 40% of the screen height
		float startY = -screenH * 0.10f;

		std::shared_ptr<Demo::IButton> buttons[] = { continueButton, newGameButton, loadGameButton, optionButton, creditButton, quitButton };
		float currentY = startY;

		for (auto& btn : buttons)
		{
			if (btn)
			{
				//center X
				float posX = -(btn->GetWidth() / 2.0f);
				btn->SetLocalPosition(posX, currentY);

				//button spacing
				currentY += btn->GetHeight() + spacingY;
			}
		}

	}

	void MainMenu::Init()
	{
		transformManager = std::make_shared<DX9GF::TransformManager>();

		auto app = DX9GF::Application::GetInstance();
		lastScreenWidth = app->GetScreenWidth();
		lastScreenHeight = app->GetScreenHeight();

		//load textures
		buttonSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		buttonSheetTex->LoadTexture(L"ui-pack.png");

		bgTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		bgTex->LoadTexture(IDB_PNG2);

		titleTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		titleTex->LoadTexture(IDB_PNG3);

		//BUTTONS INIT
		//Continue Button
		continueButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30, buttonSheetTex, 3);
		continueButton->SetSpriteCoords(577, 193, 94, 30, 34);
		continueButton->SetOnReleaseLeft([](DX9GF::ITrigger* t) { /* Logic */ });

		//New Game Button
		newGameButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30, buttonSheetTex, 3);
		newGameButton->SetSpriteCoords(577, 481, 94, 30, 34);
		newGameButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) { 
			auto app = DX9GF::Application::GetInstance();
			game->GetSceneManager()->PushScene(
				new TutorialWorldScene(game, app->GetScreenWidth(), app->GetScreenHeight())
			);
			game->GetSceneManager()->GoToNext();
		});

		//Load Game Button
		loadGameButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30, buttonSheetTex, 3);
		loadGameButton->SetSpriteCoords(577, 513, 94, 30, 34);
		loadGameButton->SetOnReleaseLeft([](DX9GF::ITrigger* t) { /* Logic */ });

		//Options Button
		optionButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 78, 30, buttonSheetTex, 3);
		optionButton->SetSpriteCoords(577, 65, 78, 30, 50);
		optionButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
			auto app = DX9GF::Application::GetInstance();
			//push Settings Scene
			this->game->GetSceneManager()->PushScene(
				new SettingsScene(this->game, app->GetScreenWidth(), app->GetScreenHeight())
			);
			this->game->GetSceneManager()->GoToNext();
			});

		//Credits Button
		creditButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 78, 30, buttonSheetTex, 3);
		creditButton->SetSpriteCoords(577, 1, 78, 30, 50);
		creditButton->SetOnReleaseLeft([](DX9GF::ITrigger* t) { /* Logic */ });

		//Quit Button
		quitButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 62, 30, buttonSheetTex, 3);
		quitButton->SetSpriteCoords(385, 449, 62, 30, 2);
		quitButton->SetOnReleaseLeft([](DX9GF::ITrigger* t) { PostQuitMessage(0); });

		//active buttons
		std::shared_ptr<Demo::IButton> buttons[] = { continueButton, newGameButton, loadGameButton, optionButton, creditButton, quitButton };
		for (auto& btn : buttons)
		{
			if (btn)
			{
				btn->Init(&camera);
				uiButtons.push_back(btn);
			}
		}

		//call it to setup the update layout
		UpdateLayout(lastScreenWidth, lastScreenHeight);

		transformManager->RebuildHierarchy();
	}

	void MainMenu::Update(unsigned long long deltaTime)
	{
		auto inpMan = DX9GF::InputManager::GetInstance();
		inpMan->ReadMouse(deltaTime);
		inpMan->ReadKeyboard(deltaTime);

		auto app = DX9GF::Application::GetInstance();
		int currentWidth = app->GetScreenWidth();
		int currentHeight = app->GetScreenHeight();

		//check the size and update if there is a change
		if (currentWidth != lastScreenWidth || currentHeight != lastScreenHeight)
		{
			UpdateLayout(currentWidth, currentHeight);
			lastScreenWidth = currentWidth;
			lastScreenHeight = currentHeight;
		}

		for (auto& button : uiButtons)
		{
			button->Update(deltaTime);
		}

		transformManager->UpdateAll();
		camera.Update();
	}

	void MainMenu::Draw(unsigned long long deltaTime)
	{
		auto gd = game->GetGraphicsDevice();
		gd->Clear();
		if (SUCCEEDED(gd->BeginDraw())) {
			if (bgSprite)
			{
				bgSprite->Begin();
				bgSprite->Draw(camera, deltaTime);
				bgSprite->End();
			}

			if (titleSprite)
			{
				titleSprite->Begin();
				titleSprite->Draw(camera, deltaTime);
				titleSprite->End();
			}

			for (auto& btn : uiButtons)
			{
				btn->Draw(gd, deltaTime);
			}
			gd->EndDraw();
		}
		gd->Present();
	}

}
