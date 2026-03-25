#include "pch.h"
#include "SettingsScene.h"
#include "resource.h"
#include "IconButton.h"

namespace Demo
{
	void SettingsScene::UpdateLayout(int screenW, int screenH)
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

		std::shared_ptr<Demo::IButton> buttons[] = { backButton };
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

	void SettingsScene::Init()
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
		backButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30,
			buttonSheetTex, 1, 417, 62, 30, 2, 
			[this](DX9GF::ITrigger* t) 
			{ 
				// Chỉ phất cờ, xin đừng tự sát ở đây!
				this->isGoingBack = true;

			}
		);


		//active buttons
		std::shared_ptr<Demo::IButton> buttons[] = {backButton};
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

	void SettingsScene::Update(unsigned long long deltaTime)
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

		if (this->isGoingBack)
		{
			auto sm = this->game->GetSceneManager();
			sm->GoToPrevious();
			sm->PopScene();
			return;
		}
	}

	void SettingsScene::Draw(unsigned long long deltaTime)
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
