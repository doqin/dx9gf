#include "pch.h"
#include "MainBlockCard.h"

void Demo::MainBlockCard::Draw(unsigned long long deltaTime)
{
    IBlockCard::Draw(deltaTime);
	if (!blockTexture) {
        blockTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		blockTexture->LoadTexture(L"ui.png");
        blockSprite = std::make_shared<DX9GF::StaticSprite>(blockTexture.get());
		blockSprite->SetSrcRect({ .left = 0, .top = 272, .right = 80, .bottom = 288 });
    }
	blockSprite->Begin();
	blockSprite->SetPosition(GetWorldX(), GetWorldY());
	blockSprite->SetScale(2.f, 2.f);
	blockSprite->Draw(*camera, deltaTime);
	blockSprite->End();
    if (!nameFont) {
        nameFont = std::make_shared<DX9GF::Font>(graphicsDevice, L"StatusPlz", 16);
        nameFontSprite = std::make_shared<DX9GF::FontSprite>(nameFont.get());
        nameFontSprite->SetColor(0xFF000000);
    }
    nameFontSprite->Begin();
    //nameFontSprite->SetPosition(GetWorldX() + 8.f, GetWorldY() + 8.f);
    //nameFontSprite->SetText(L"Main block");
    //nameFontSprite->Draw(*camera, deltaTime);
    if (auto currentCard = GetCurrentExecutingCard()) {
        nameFontSprite->SetPosition(currentCard->GetWorldX() - 24.f, currentCard->GetWorldY() + 8.f);
        nameFontSprite->SetText(L"->");
        nameFontSprite->Draw(*camera, deltaTime);
    }
    nameFontSprite->End();
}
