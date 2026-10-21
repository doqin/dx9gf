#include "pch.h"
#include "IBlockCard.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include "IBattleScene.h"
#include "DrawUtils.h"
#include <algorithm>

bool Demo::IBlockCard::AffordOrReturnToHand(const std::shared_ptr<IStatementCard>& card)
{
	if (!battleScene || battleScene->CanPlaceCardInBlock(card)) {
		return true;
	}
	if (timeSinceLastEnergyPopUp >= energyPopUpCooldown) {
		battleScene->QueuePopUpMessage(Tr(L"Not enough energy"));
		timeSinceLastEnergyPopUp = 0.f;
	}
	battleScene->ReturnCardToHand(card);
	return false;
}

void Demo::IBlockCard::RemoveFromQueue(const IStatementCard* card)
{
	for (size_t i = 0; i < statementCards.size(); ++i) {
		if (statementCards[i].lock().get() == card) {
			statementCards.erase(statementCards.begin() + i);
			if (executeIndex > i) {
				--executeIndex;
			}
			break;
		}
	}
	children.erase(std::remove_if(children.begin(), children.end(),
		[&](const std::weak_ptr<IDraggable>& weak) { return weak.lock().get() == card; }),
		children.end());
}

size_t Demo::IBlockCard::DropIndexAt(float dropY, const std::shared_ptr<IStatementCard>& dropped, float* slotTopY)
{
	size_t index = 0;
	for (const auto& weak : statementCards) {
		auto card = weak.lock();
		if (!card || card == dropped) {
			continue;
		}
		const float top = card->GetWorldY();
		if (top + static_cast<float>(card->GetHeight()) / 2.0f >= dropY) {
			if (slotTopY) *slotTopY = top;
			break;
		}
		++index;
	}
	return index;
}

bool Demo::IBlockCard::OnDrop(std::shared_ptr<IDraggable> other)
{
	auto statementCard = std::dynamic_pointer_cast<IStatementCard>(other);
	if (!statementCard || !AffordOrReturnToHand(statementCard)) {
		return false;
	}

	// Measured before IContainer::OnDrop, which snaps the card to the end of the list.
	const float dropY = other->GetWorldY() + other->GetHeight() / 2.0f;

	if (IContainer::OnDrop(other)) {
		// Queue the card at the slot it was dropped on rather than always last, so queued cards can
		// be reordered by dragging them within the block. A running block keeps appending, so
		// the execution pointer never shifts under it. RemoveFromQueue clears the entry
		// IContainer::OnDrop just appended to `children` and any stale one in `statementCards`, so
		// neither list holds the card twice (double execution, double energy commit, double discard).
		size_t index = isExecuting ? statementCards.size() : DropIndexAt(dropY, statementCard);
		RemoveFromQueue(statementCard.get());
		index = (std::min)({ index, statementCards.size(), children.size() });
		statementCards.insert(statementCards.begin() + index, statementCard);
		children.insert(children.begin() + index, statementCard);
		return true;
	}
	return false;
}

void Demo::IBlockCard::Update(unsigned long long deltaTime)
{
	IContainer::Update(deltaTime);

	timeSinceLastEnergyPopUp += deltaTime / 1000.f;

	// Drop cards that expired or were moved to another parent.
	for (size_t i = 0; i < statementCards.size();) {
		auto card = statementCards[i].lock();
		if (card) {
			auto parent = card->GetParent();
			if (parent.has_value() && parent.value().lock().get() == this) {
				++i;
				continue;
			}
		}
		statementCards.erase(statementCards.begin() + i);
		if (executeIndex > i) {
			--executeIndex;
		}
	}
	if (isExecuting) {
		ExecuteIteratively(deltaTime);
	}
}

void Demo::IBlockCard::Draw(unsigned long long deltaTime)
{
	IContainer::Draw(deltaTime);
	if (!blockTexture) {
		blockTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		blockTexture->LoadTexture(L"assets/ui.png");
		blockSprite = std::make_shared<DX9GF::StaticSprite>(blockTexture.get());
		blockSprite->SetSrcRect(GetBlockFaceRect());
		pointerSprite = std::make_shared<DX9GF::StaticSprite>(blockTexture.get());
		pointerSprite->SetSrcRect({ .left = 128, .top = 0, .right = 144, .bottom = 16 });
		pointerSprite->SetScale(2.f, 2.f);
		pointerSprite->SetOrigin(8.f, 8.f);
	}
	blockSprite->Begin();
	blockSprite->SetPosition(GetWorldX(), GetWorldY());
	blockSprite->SetScale(2.f, 2.f);
	blockSprite->Draw(*camera, deltaTime);
	blockSprite->End();
	if (!nameFont) {
		nameFont = std::make_shared<DX9GF::Font>(graphicsDevice, Demo::kMainFontName, Demo::kMainFontSize);
		nameFontSprite = std::make_shared<DX9GF::FontSprite>(nameFont.get());
		nameFontSprite->SetColor(0xFF000000);
	}
	if (auto currentCard = GetCurrentExecutingCard()) {
		pointerSprite->Begin();
		pointerSprite->SetPosition(currentCard->GetWorldX() - 32.f, currentCard->GetWorldY() + 16.f);
		pointerSprite->Draw(*camera, deltaTime);
		pointerSprite->End();
	}
	for (auto& draggable : draggableManager->GetDraggingDraggables()) {
		if (auto lock = draggable->GetParent(); lock.has_value()) {
			if (auto parentLock = lock.value().lock()) {
				if (parentLock.get() == this) {
					continue;
				}
			}
		}
		if (auto draggedStatementCard = std::dynamic_pointer_cast<Demo::IStatementCard>(draggable)) {
			auto thisX = GetWorldX();
			auto thisY = GetWorldY() + GetHeight() + GetHeightOfChildren();
			auto width = (std::max)(this->GetMaxWidthOfChildren(), GetWidth());
			auto height = 32.f;
			// Over the queued cards: mark the gap OnDrop will insert into, instead of the end slot.
			const float dragCenterX = draggedStatementCard->GetWorldX() + draggedStatementCard->GetWidth() / 2.0f;
			const float dragCenterY = draggedStatementCard->GetWorldY() + draggedStatementCard->GetHeight() / 2.0f;
			const bool overQueuedCards = !isExecuting
				&& dragCenterX > thisX && dragCenterX < thisX + width
				&& dragCenterY > GetWorldY() + static_cast<float>(dragAreaHeight) && dragCenterY < thisY;
			if (overQueuedCards) {
				constexpr float NO_SLOT = -1e9f;
				float slotY = NO_SLOT;
				DropIndexAt(dragCenterY, draggedStatementCard, &slotY);
				// NO_SLOT means the card would land at the end, which the dashed slot below marks.
				if (slotY != NO_SLOT) {
					const float barX = thisX;
					const float barW = static_cast<float>(width);
					draggableManager->QueueDraw(std::make_shared<DX9GF::CustomCommand>([this, barX, slotY, barW](std::function<void(void)> markFinished) {
						graphicsDevice->DrawRectangle(*camera, barX, slotY - 2.f, barW, 4.f, 0xFFFFD700, true);
						markFinished();
						}));
					return;
				}
			}
			draggableManager->QueueDraw(std::make_shared<DX9GF::CustomCommand>([&, width, height, thisX, thisY](std::function<void(void)> markFinished) {
				graphicsDevice->SetAlphaBlending(true);
				Demo::DrawAnimatedDashedRectangle(
					graphicsDevice,
					*camera,
					thisX,
					thisY,
					width,
					height,
					3.f,
					0xFFFFFFFF,
					false,
					4.f,
					0xFFFFFFFF,
					20.f,
					10.f,
					40.f,
					GetTickCount64()
				);
				graphicsDevice->SetAlphaBlending(false);
				markFinished();
				}));
			return;
		}
	}
}

void Demo::IBlockCard::StartExecution()
{
	executeIndex = 0;
	currentExecutingCard.reset();
	for (auto& card : statementCards) {
		if (auto lock = card.lock()) {
			lock->ResetExecution();
		}
	}
	isExecuting = !statementCards.empty();
}

void Demo::IBlockCard::ForgetStatementCard(const std::shared_ptr<IStatementCard>& card)
{
	if (!card) {
		return;
	}
	RemoveFromQueue(card.get());
	if (auto parent = card->GetParent(); parent.has_value() && parent.value().lock().get() == this) {
		card->DetachParent();
	}
}

void Demo::IBlockCard::ExecuteIteratively(unsigned long long deltaTime)
{
	if (executeIndex >= statementCards.size()) {
		isExecuting = false;
		currentExecutingCard.reset();
		return;
	}
	timeSinceLastExecution += deltaTime / 1000.f;
	if (timeSinceLastExecution <= timePerExecution) {
		return;
	}
	else {
		timeSinceLastExecution = 0;
	}
	auto current = statementCards[executeIndex].lock();
	if (!current) {
		++executeIndex;
		currentExecutingCard.reset();
		return;
	}
	currentExecutingCard = current;
	if (current->Execute()) {
		// A card queued in the block runs again on every following turn until the program
		// clears, so a use is spent per completed execution, not per time it was played.
		current->ConsumeUse();
		++executeIndex;
	}
}

bool Demo::IBlockCard::IsExecuting() const
{
	return isExecuting;
}

std::shared_ptr<Demo::IStatementCard> Demo::IBlockCard::GetCurrentExecutingCard() const
{
    return currentExecutingCard.lock();
}

void Demo::IBlockCard::ResetExecution()
{
	executeIndex = 0;
	isExecuting = false;
	currentExecutingCard.reset();
}

bool Demo::IBlockCard::InsertStatementCardAt(std::shared_ptr<IStatementCard> card, size_t index)
{
	if (!card) {
		return false;
	}

	const bool alreadyAttached = std::any_of(statementCards.begin(), statementCards.end(),
		[&](const std::weak_ptr<IStatementCard>& weak) { return weak.lock() == card; });
	if (!alreadyAttached && !AffordOrReturnToHand(card)) {
		return false;
	}

	// Reordering: take the card out first so the index below counts only the other cards.
	RemoveFromQueue(card.get());

	index = (std::min)(index, statementCards.size());
	card->SetParent(shared_from_this());
	statementCards.insert(statementCards.begin() + index, card);
	children.insert(children.begin() + index, card);
	// IContainer::Update repositions all children by vector order every frame, so no explicit reposition needed here.
	return true;
}

std::tuple<float, float> Demo::IBlockCard::GetStatementSlotWorldPosition(size_t index, std::shared_ptr<IStatementCard> excluding)
{
	auto [thisX, thisY] = GetWorldPosition();
	float y = thisY + (float)dragAreaHeight;
	size_t counted = 0;
	for (auto& weak : statementCards) {
		auto lock = weak.lock();
		if (!lock || lock == excluding) {
			continue;
		}
		if (counted == index) {
			break;
		}
		y += (float)lock->GetHeight();
		++counted;
	}
	return { thisX, y };
}

bool Demo::IBlockCard::HasAllRequiredTargets() const
{
	for (auto& card : statementCards) {
		if (auto lock = card.lock()) {
			if (!lock->HasRequiredTargets()) {
				return false;
			}
		}
	}
	return true;
}

bool Demo::IBlockCard::TryReclaim(std::weak_ptr<DX9GF::IGameObject> oldParent)
{
	// Block cards don't belong to any container and have no original position. 
	// An invalid drop will just keep the card at its current dropped position instead of reverting.
	return true;
}