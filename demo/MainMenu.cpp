#include "pch.h"
#include "MainMenu.h"
// Include các class cụ thể kế thừa từ IButton (ví dụ: TextButton, IconButton)
#include "resource.h"
#include "IconButton.h"
namespace Demo
{
	void MainMenu::UpdateLayout(int screenW, int screenH)
	{
		camera.SetPosition(screenW / 2.0f, screenH / 2.0f);

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
		bgSprite->SetPosition(screenW / 2.0f, screenH / 2.0f);

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
		titleSprite->SetPosition(screenW / 2.0f, screenH * 0.3f);

		//BUTTONS: use anchor center
		//find center location
		float centerX = screenW / 2.0f;
		float centerY = screenH / 2.0f;
		//spacing between buttons
		float spacingY = 10.0f;
		//start drawing from 40% of the screen height
		float startY = screenH * 0.40f;

		std::shared_ptr<Demo::IButton> buttons[] = { continueButton, newGameButton, loadGameButton, optionButton, creditButton, quitButton };
		float currentY = startY;

		for (auto& btn : buttons)
		{
			if (btn)
			{
				//center X
				float posX = centerX - (btn->GetWidth() / 2.0f);
				btn->SetPosition(posX, currentY);

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

		//buttons init
		continueButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30,
			buttonSheetTex, 577, 193, 94, 30, 34, [](DX9GF::ITrigger* t) { /* Logic */ });

		newGameButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30,
			buttonSheetTex, 577, 481, 94, 30, 34, [](DX9GF::ITrigger* t) { /* Logic */ });

		loadGameButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30,
			buttonSheetTex, 577, 513, 94, 30, 34, [](DX9GF::ITrigger* t) { /* Logic */ });

		optionButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 78, 30,
			buttonSheetTex, 577, 65, 78, 30, 50, [](DX9GF::ITrigger* t) { /* Logic */ });

		creditButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 78, 30,
			buttonSheetTex, 577, 1, 78, 30, 50, [](DX9GF::ITrigger* t) { /* Logic */ });

		quitButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 62, 30,
			buttonSheetTex, 385, 449, 62, 30, 2, [](DX9GF::ITrigger* t) { PostQuitMessage(0); });

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
				btn->Draw(&this->camera, gd, deltaTime);
			}
			gd->EndDraw();
		}
		gd->Present();
	}

}
