#include "pch.h"
#include "InventoryMenu.h"
#include "SettingsScene.h"

namespace Demo {

	InventoryMenu::InventoryMenu(Game* g, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::TransformManager> tm, std::shared_ptr<DraggableManager> dm, DX9GF::Camera* cam, DX9GF::Font* f)
		: game(g), player(p), transformManager(tm), draggableManager(dm), uiCamera(cam), font(f)
	{
		fontSprite = std::make_shared<DX9GF::FontSprite>(f);
	}

	void InventoryMenu::Init()
	{
		auto [sw, sh] = uiCamera->GetScreenResolution();
		float centerX = 0.0f;
		float topY = -sh / 2.0f;
		float bottomY = sh / 2.0f;
		float tabY = topY + 40.0f;
		float tabGap = 24.0f;
		float bottomGap = 20.0f;
		float containerGap = 40.0f;
		float containerW = (sw - 120.0f - containerGap) / 2.0f;
		float containerY = topY + 110.0f;
		float leftContainerX = -containerGap / 2.0f - containerW;
		float rightContainerX = containerGap / 2.0f;
		float resumeX = -(70.0f + bottomGap + 80.0f + bottomGap + 70.0f) / 2.0f;
		float optionsX = resumeX + 70.0f + bottomGap;
		float leaveX = optionsX + 80.0f + bottomGap;

        btnTabItems = std::make_shared<TextButton>(transformManager, centerX - tabGap / 2.0f - 80.0f, tabY, 80.0f, 30.0f, std::string("ITEMS"), this->font,
			[this](DX9GF::ITrigger* t) { this->SetTab(Tab::ITEMS); });
		btnTabItems->SetBackgroundColors(D3DXCOLOR(0.2f, 0.2f, 0.2f, 1), D3DXCOLOR(0.4f, 0.4f, 0.4f, 1), D3DXCOLOR(0.8f, 0.5f, 0.1f, 1));
		btnTabItems->Init(uiCamera);

       btnTabDeck = std::make_shared<TextButton>(transformManager, centerX + tabGap / 2.0f, tabY, 80.0f, 30.0f, std::string("DECK"), this->font,
			[this](DX9GF::ITrigger* t) { this->SetTab(Tab::DECK); });
		btnTabDeck->SetBackgroundColors(D3DXCOLOR(0.2f, 0.2f, 0.2f, 1), D3DXCOLOR(0.4f, 0.4f, 0.4f, 1), D3DXCOLOR(0.8f, 0.5f, 0.1f, 1));
		btnTabDeck->Init(uiCamera);

      btnResume = std::make_shared<TextButton>(transformManager, resumeX, bottomY - 50.0f, 70.0f, 30.0f, std::string("RESUME"), this->font,
			[this](DX9GF::ITrigger* t) { this->Toggle(); });
		btnResume->SetBackgroundColors(D3DXCOLOR(0.1f, 0.5f, 0.1f, 1), D3DXCOLOR(0.2f, 0.7f, 0.2f, 1), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1));
		btnResume->Init(uiCamera);

     btnOptions = std::make_shared<TextButton>(transformManager, optionsX, bottomY - 50.0f, 80.0f, 30.0f, std::string("OPTIONS"), this->font,
			[this, sw, sh](DX9GF::ITrigger* t) {
				this->game->GetSceneManager()->PushScene(new SettingsScene(this->game, sw, sh));
				this->game->GetSceneManager()->GoToNext();
			});
		btnOptions->Init(uiCamera);

        btnLeaveGame = std::make_shared<TextButton>(transformManager, leaveX, bottomY - 50.0f, 70.0f, 30.0f, std::string("LEAVE"), this->font,
			[this](DX9GF::ITrigger* t) {
				this->pendingLeave = true;
			});
		btnLeaveGame->SetBackgroundColors(D3DXCOLOR(0.6f, 0.1f, 0.1f, 1), D3DXCOLOR(0.8f, 0.2f, 0.2f, 1), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1));
		btnLeaveGame->Init(uiCamera);

       deckContainer = std::make_shared<IContainer>(transformManager, containerW, 40.0f, leftContainerX, containerY);
		deckContainer->Init(draggableManager, game->GetGraphicsDevice(), uiCamera);

        inventoryContainer = std::make_shared<IContainer>(transformManager, containerW, 40.0f, rightContainerX, containerY);
		inventoryContainer->Init(draggableManager, game->GetGraphicsDevice(), uiCamera);
	}

	void InventoryMenu::Toggle()
	{
		isOpen = !isOpen;
		if (isOpen) {
			for (auto& cardId : player->GetDeck()) {
				auto dragCard = std::dynamic_pointer_cast<IDraggable>(ICard::CreateCard(cardId, transformManager, draggableManager, game->GetGraphicsDevice(), uiCamera));
				if (dragCard) deckContainer->AddChildProgrammatically(dragCard);
			}

			for (auto& cardId : player->GetInventoryCards()) {
				auto dragCard = std::dynamic_pointer_cast<IDraggable>(ICard::CreateCard(cardId, transformManager, draggableManager, game->GetGraphicsDevice(), uiCamera));
				if (dragCard) inventoryContainer->AddChildProgrammatically(dragCard);
			}
		}
		else {
			player->ClearDeck();
			for (auto& weakChild : deckContainer->GetChildren()) {
				if (auto child = weakChild.lock()) {
					child->DetachParent();
					if (auto card = std::dynamic_pointer_cast<ICard>(child)) {
						player->AddCardToDeck(card->GetSaveID());
					}
					draggableManager->Remove(child);
				}
			}
			deckContainer->ClearChildren();

			player->ClearInventory();
			for (auto& weakChild : inventoryContainer->GetChildren()) {
				if (auto child = weakChild.lock()) {
					child->DetachParent();
					if (auto card = std::dynamic_pointer_cast<ICard>(child)) {
						player->AddCardToInventory(card->GetSaveID());
					}
					draggableManager->Remove(child);
				}
			}
			inventoryContainer->ClearChildren();
		}

		if (transformManager) {
			transformManager->RebuildHierarchy();
		}
	}

	void InventoryMenu::Update(unsigned long long deltaTime)
	{
		if (!isOpen) return;

		auto [sw, sh] = uiCamera->GetScreenResolution();
		float centerX = 0.0f;
		float topY = -sh / 2.0f;
		float bottomY = sh / 2.0f;
       float tabY = topY + 40.0f;
		float tabGap = 24.0f;
		float bottomGap = 20.0f;
		float containerGap = 40.0f;
		float containerW = (sw - 120.0f - containerGap) / 2.0f;
		float containerY = topY + 110.0f;
		float leftContainerX = -containerGap / 2.0f - containerW;
		float rightContainerX = containerGap / 2.0f;
		float resumeX = -(70.0f + bottomGap + 80.0f + bottomGap + 70.0f) / 2.0f;
		float optionsX = resumeX + 70.0f + bottomGap;
		float leaveX = optionsX + 80.0f + bottomGap;

		btnTabItems->SetLocalPosition(centerX - tabGap / 2.0f - 80.0f, tabY);
		btnTabDeck->SetLocalPosition(centerX + tabGap / 2.0f, tabY);
		btnResume->SetLocalPosition(resumeX, bottomY - 50.0f);
		btnOptions->SetLocalPosition(optionsX, bottomY - 50.0f);
		btnLeaveGame->SetLocalPosition(leaveX, bottomY - 50.0f);

		deckContainer->SetLocalPosition(leftContainerX, containerY);
		inventoryContainer->SetLocalPosition(rightContainerX, containerY);

		btnTabItems->Update(deltaTime);
		btnTabDeck->Update(deltaTime);
		btnResume->Update(deltaTime);
		btnOptions->Update(deltaTime);
		btnLeaveGame->Update(deltaTime);

		if (currentTab == Tab::DECK) {
			deckContainer->Update(deltaTime);
			inventoryContainer->Update(deltaTime);
		}
	}

	void InventoryMenu::Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
	{
		if (!isOpen) return;

		auto [sw, sh] = uiCamera->GetScreenResolution();
		float centerX = 0.0f;
		float leftEdge = -sw / 2.0f;
		float topEdge = -sh / 2.0f;
		float bottomEdge = sh / 2.0f;

		gd->DrawRectangle(*uiCamera, leftEdge, topEdge, sw, sh, 0, 1, 1, 0, 0, D3DXCOLOR(0, 0, 0, 0.85f), true);

		btnTabItems->Draw(gd, deltaTime);
		btnTabDeck->Draw(gd, deltaTime);
		btnResume->Draw(gd, deltaTime);
		btnOptions->Draw(gd, deltaTime);
		btnLeaveGame->Draw(gd, deltaTime);

		fontSprite->Begin();
		fontSprite->SetScale(1.5f, 1.5f);
		fontSprite->SetColor(0xFFFFFF00);
     fontSprite->SetPosition(leftEdge + 30.0f, bottomEdge - 95.0f);

        fontSprite->SetText(std::to_wstring(player->GetGold()) + L"G");
		fontSprite->Draw(*uiCamera, deltaTime);

     fontSprite->SetPosition(leftEdge + 30.0f, bottomEdge - 60.0f);
     fontSprite->SetText(std::to_wstring(static_cast<int>(player->GetHealth())) + L"/" + std::to_wstring(static_cast<int>(player->GetMaxHealth())) + L"HP");
		fontSprite->Draw(*uiCamera, deltaTime);
		fontSprite->End();

		if (currentTab == Tab::ITEMS) {
			fontSprite->Begin();
			fontSprite->SetScale(2.0f, 2.0f);
			fontSprite->SetColor(0xFFFFFFFF);
			fontSprite->SetPosition(centerX - 80.0f, 0.0f);
			fontSprite->SetText(L"No Items Yet");
			fontSprite->Draw(*uiCamera, deltaTime);
			fontSprite->End();
		}
		else if (currentTab == Tab::DECK) {
           float containerGap = 40.0f;
			float containerW = (sw - 120.0f - containerGap) / 2.0f;
			float leftContainerX = -containerGap / 2.0f - containerW;
			float rightContainerX = containerGap / 2.0f;

			fontSprite->Begin();
			fontSprite->SetScale(1.0f, 1.0f);
			fontSprite->SetColor(0xFFFFFFFF);

           fontSprite->SetPosition(leftContainerX + 10.0f, topEdge + 80.0f);
			fontSprite->SetText(L"Current Deck");
			fontSprite->Draw(*uiCamera, deltaTime);

            fontSprite->SetPosition(rightContainerX + 10.0f, topEdge + 80.0f);
			fontSprite->SetText(L"Available Cards");
			fontSprite->Draw(*uiCamera, deltaTime);
			fontSprite->End();

			deckContainer->Draw(deltaTime);
			inventoryContainer->Draw(deltaTime);
		}
	}
}