#include "pch.h"
#include "HandContainer.h"
#include "IBlockCard.h"
#include <algorithm>

void Demo::HandContainer::Init(std::shared_ptr<DraggableManager> manager, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera)
{
	IContainer::Init(manager, graphicsDevice, camera);
}

void Demo::HandContainer::Init(std::shared_ptr<DraggableManager> manager, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::shared_ptr<IDraggable>>* playedPile)
{
	this->Init(manager, graphicsDevice, camera);
	this->playedPile = playedPile;
	if (trigger) {
		trigger->SetOnHeldLeft([](DX9GF::ITrigger* thisObj) {
			});
		trigger->SetOnReleaseLeft([](DX9GF::ITrigger* thisObj) {
			});
	}
}

bool Demo::HandContainer::OnDrop(std::shared_ptr<IDraggable> other)
{
	if (!std::dynamic_pointer_cast<IStatementCard>(other)) {
		return false;
	}

	if (std::find_if(playedPile->begin(), playedPile->end(), [&](const std::weak_ptr<IDraggable>& playedCard) {
		auto lock = playedCard.lock();
		return lock && lock.get() == other.get();
	}) != playedPile->end()) {
		return false;
	}
	return IContainer::OnDrop(other);
}

void Demo::HandContainer::StoreCard(std::shared_ptr<IDraggable> card)
{
	if (!card) {
		return;
	}
  if (!std::dynamic_pointer_cast<IStatementCard>(card)) {
		return;
	}
	if (auto parent = card->GetParent(); parent.has_value()) {
		card->DetachParent();
	}

	children.erase(std::remove_if(children.begin(), children.end(), [&](const std::weak_ptr<IDraggable>& child) {
		auto lock = child.lock();
		return !lock || lock.get() == card.get();
	}), children.end());

	float yPos = static_cast<float>(dragAreaHeight);
	for (const auto& child : children) {
		if (auto lock = child.lock()) {
			yPos += static_cast<float>(lock->GetHeight());
		}
	}

	card->SetParent(shared_from_this());
	card->SetLocalPosition(0.f, yPos);
	children.push_back(card);
}
