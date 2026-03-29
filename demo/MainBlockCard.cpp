#include "pch.h"
#include "MainBlockCard.h"

void Demo::MainBlockCard::Draw(unsigned long long deltaTime)
{
    IBlockCard::Draw(deltaTime);
    if (!nameFont) {
        nameFont = std::make_shared<DX9GF::Font>(graphicsDevice, L"Arial", 16);
        nameFontSprite = std::make_shared<DX9GF::FontSprite>(nameFont.get());
    }
    nameFontSprite->Begin();
    nameFontSprite->SetPosition(GetWorldX() + 8.f, GetWorldY() + 8.f);
    nameFontSprite->SetText(L"MainBlockCard");
    nameFontSprite->Draw(*camera, deltaTime);
    if (auto currentCard = GetCurrentExecutingCard()) {
        nameFontSprite->SetPosition(currentCard->GetWorldX() - 24.f, currentCard->GetWorldY() + 8.f);
        nameFontSprite->SetText(L"->");
        nameFontSprite->Draw(*camera, deltaTime);
    }
    nameFontSprite->End();
}
