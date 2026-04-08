#include "pch.h"
#include "EnemyCard.h"

std::shared_ptr<Demo::IEnemy> Demo::EnemyCard::GetValue()
{
	return enemy;
}

void Demo::EnemyCard::Draw(unsigned long long deltaTime)
{
	IExpressionCard::Draw(deltaTime);
	if (!nameFont) {
		nameFont = std::make_shared<DX9GF::Font>(graphicsDevice, L"StatusPlz", 16);
		nameFontSprite = std::make_shared<DX9GF::FontSprite>(nameFont.get());
		nameFontSprite->SetColor(0xFF000000);
	}
	nameFontSprite->Begin();
	nameFontSprite->SetPosition(GetWorldX() + 8.f, GetWorldY() + 8.f);
	nameFontSprite->SetText(L"EnemyCard");
	nameFontSprite->Draw(*camera, deltaTime);
	nameFontSprite->End();
}
