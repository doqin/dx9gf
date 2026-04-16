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

		arrowTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		arrowTexture->LoadTexture(L"arrow.png");
		arrowSprite = std::make_shared<DX9GF::StaticSprite>(arrowTexture.get());
		arrowSprite->SetScale(2.f);
		auto [w, h] = arrowTexture->GetSize();
		arrowSprite->SetOrigin(w / 2.0f, h / 2.0f);
	}
	nameFontSprite->Begin();
	nameFontSprite->SetPosition(GetWorldX() + 8.f, GetWorldY() + 8.f);
	nameFontSprite->SetText(L"EnemyCard");
	nameFontSprite->Draw(*camera, deltaTime);
	nameFontSprite->End();

	const float x = GetWorldX() + GetWidth() / 2.f;
	const float y = GetWorldY() + GetHeight() / 2.f;
	const float enemyX = enemy->GetWorldX();
	const float enemyY = enemy->GetWorldY();
	float dx = enemyX - x;
	float dy = enemyY - y;
	if (trigger->IsHovering(deltaTime)) {
		graphicsDevice->DrawLine(*camera, x, y, enemyX, enemyY, 0xFF000000, 8.0f);
		graphicsDevice->DrawLine(*camera, x, y, enemyX, enemyY, 0xFFFFFFFF, 6.0f);
		graphicsDevice->DrawLine(*camera, x, y, enemyX, enemyY, 0xFFB4202A, 4.0f);
	}
	float angle = atan2(dy, dx);
	arrowSprite->SetPosition(x, y);
	arrowSprite->SetRotation(angle);

	arrowSprite->Begin();
	arrowSprite->Draw(*camera, deltaTime);
	arrowSprite->End();
}
