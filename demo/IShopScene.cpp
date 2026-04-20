#include "pch.h"
#include "IShopScene.h"

Demo::IShopScene::IShopScene(Game* game, Player* player, int sw, int sh, std::string title)
	: IScene(sw, sh), game(game), player(player), uiCamera(sw, sh), shopTitle(title)
{
	uiCamera.SetPosition(sw / 2.0f, sh / 2.0f);
}

void Demo::IShopScene::Init()
{
	transformManager = std::make_shared<DX9GF::TransformManager>();
	myFont = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 24);
	myFontSprite = std::make_shared<DX9GF::FontSprite>(myFont.get());

	LoadItems();

	BuildUI();

	transformManager->RebuildHierarchy();
}

void Demo::IShopScene::BuildUI()
{
	float startY = 160.0f;
	for (size_t i = 0; i < itemsForSale.size(); ++i) {
		auto item = itemsForSale[i];

		auto buyBtn = std::make_shared<Demo::TextButton>(
			transformManager,
			450, startY + (i * 80), 100, 40,
			"BUY",
			myFont.get(),
			[this, item](DX9GF::ITrigger* t) {
				if (this->player->GetGold() >= item.cost) {
					this->player->AddGold(-item.cost);

					if (item.onBuyAction) {
						item.onBuyAction();
					}

					this->ShowMessage("Bought " + item.name + "!");
				}
				else {
					this->ShowMessage("Not enough gold!");
				}
			}
		);
		buyBtn->Init(&uiCamera);
		uiButtons.push_back(buyBtn);
	}

	auto leaveBtn = std::make_shared<Demo::TextButton>(
		transformManager,
		20, 20, 100, 40,
		"LEAVE",
		myFont.get(),
		[this](DX9GF::ITrigger* t) {
			this->shouldLeave = true;
		}
	);
	leaveBtn->Init(&uiCamera);
	uiButtons.push_back(leaveBtn);
}

void Demo::IShopScene::ShowMessage(std::string msg) {
	statusMessage = msg;
	messageTimer = 2.0f;
}

void Demo::IShopScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
	uiCamera.Update();

	if (messageTimer > 0) {
		messageTimer -= deltaTime / 1000.0f;
		if (messageTimer <= 0) statusMessage = "";
	}

	for (auto& btn : uiButtons) {
		btn->Update(deltaTime);
	}

	if (shouldLeave) {
		auto sceMan = this->game->GetSceneManager();

		sceMan->PopScene();
		sceMan->GoToPrevious();
	}
}

void Demo::IShopScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear();

	if (SUCCEEDED(gd->BeginDraw())) {
		for (auto& btn : uiButtons) {
			btn->Draw(gd, deltaTime);
		}

		if (myFontSprite) {
			myFontSprite->Begin();

			myFontSprite->SetPosition(600.0f, 20.0f);
			myFontSprite->SetColor(0xFFFFD700);
			std::string goldText = "Gold: " + std::to_string(player->GetGold());
			myFontSprite->SetText(std::wstring(goldText.begin(), goldText.end()));
			myFontSprite->Draw(uiCamera, deltaTime);

			myFontSprite->SetPosition(350.0f, 50.0f);
			myFontSprite->SetColor(0xFFFFFFFF);
			myFontSprite->SetText(std::wstring(shopTitle.begin(), shopTitle.end()));
			myFontSprite->Draw(uiCamera, deltaTime);

			myFontSprite->SetPosition(350.0f, 400.0f);
			myFontSprite->SetColor(0xFF00FFFF);
			myFontSprite->SetText(std::wstring(statusMessage.begin(), statusMessage.end()));
			myFontSprite->Draw(uiCamera, deltaTime);

			float startY = 170.0f;
			for (size_t i = 0; i < itemsForSale.size(); ++i) {
				myFontSprite->SetPosition(100.0f, startY + (i * 80));
				myFontSprite->SetColor(0xFFFFFFFF);

				std::string itemText = itemsForSale[i].name + " - Cost: " + std::to_string(itemsForSale[i].cost);
				myFontSprite->SetText(std::wstring(itemText.begin(), itemText.end()));
				myFontSprite->Draw(uiCamera, deltaTime);
			}

			myFontSprite->End();
		}
		auto [sw, sh] = uiCamera.GetScreenResolution();
		DX9GF::Camera cursorCam(sw, sh);
		DX9GF::InputManager::GetInstance()->DrawCursor(&cursorCam, deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}