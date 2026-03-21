#include "pch.h"
#include "StrikeCard.h"

bool Demo::StrikeCard::OnDrop(std::shared_ptr<IDraggable> other)
{
	auto incomingEnemyCard = std::dynamic_pointer_cast<EnemyCard>(other);
	if (!incomingEnemyCard) {
		return false;
	}
	auto [thisX, thisY] = this->GetWorldPosition();
	auto [otherX, otherY] = other->GetWorldPosition();
	if (otherX > thisX
		&& otherX < thisX + dragAreaWidth
		&& otherY > thisY
		&& otherY < thisY + dragAreaHeight) {
		other->SetParent(shared_from_this());
		other->SetLocalPosition(dragAreaWidth, 0);
		enemyCard = incomingEnemyCard;
		return true;
	}
	return false;
}

bool Demo::StrikeCard::Execute()
{
	if (isDone) {
		return true;
	}
	if (auto lock = enemyCard.lock()) {
		if (auto enemy = lock->GetValue()) {
			enemy->TakeDamage(5.f);
		}
	}
	isDone = true;
	return true;
}

void Demo::StrikeCard::ResetExecution()
{
	isDone = false;
}

void Demo::StrikeCard::Draw(unsigned long long deltaTime)
{
	IStatementCard::Draw(deltaTime);
	if (!nameFont) {
		nameFont = std::make_shared<DX9GF::Font>(graphicsDevice, L"Arial", 16);
		nameFontSprite = std::make_shared<DX9GF::FontSprite>(nameFont.get());
	}
	nameFontSprite->Begin();
	nameFontSprite->SetPosition(GetWorldX() + 8.f, GetWorldY() + 8.f);
	nameFontSprite->SetText(L"StrikeCard");
	nameFontSprite->Draw(*camera, deltaTime);
	nameFontSprite->End();
}
