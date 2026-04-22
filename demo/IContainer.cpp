#include "pch.h"
#include "IContainer.h"

size_t Demo::IContainer::GetMaxWidthOfChildren()
{
	size_t maxWidth = 0;
	for (auto& child : children) {
		if (auto lock = child.lock()) {
			maxWidth = (std::max)(maxWidth, lock->GetWidth());
		}
	}
	return maxWidth;
}

size_t Demo::IContainer::GetHeightOfChildren()
{
	size_t height = 0;
	for (auto& child : children) {
		if (auto lock = child.lock()) {
			height += lock->GetHeight();
		}
	}
	return height;
}

bool Demo::IContainer::OnHover(std::shared_ptr<IDraggable> other)
{
	auto [thisX, thisY] = this->GetWorldPosition();
	auto [otherX, otherY] = other->GetWorldPosition();
	if (otherX > thisX
		&& otherX < thisX + (std::max)(dragAreaWidth, GetMaxWidthOfChildren())
		&& otherY > thisY + dragAreaHeight
		&& otherY < thisY + 2 * dragAreaHeight + GetHeightOfChildren()) {
		isHovered = true;
		return true;
	}
	isHovered = false;
	return false;
}

bool Demo::IContainer::OnDrop(std::shared_ptr<IDraggable> other)
{
	auto [thisX, thisY] = this->GetWorldPosition();
	auto [otherX, otherY] = other->GetWorldPosition();
	if (otherX > thisX
		&& otherX < thisX + (std::max)(dragAreaWidth, GetMaxWidthOfChildren())
		&& otherY > thisY + dragAreaHeight
		&& otherY < thisY + 2 * dragAreaHeight + GetHeightOfChildren()) {
		other->SetParent(shared_from_this());
		other->SetLocalPosition(0, dragAreaHeight + GetHeightOfChildren());
		children.push_back(other);
		return true;
	}
	return false;
}

void Demo::IContainer::Update(unsigned long long deltaTime)
{
	IDraggable::Update(deltaTime);
	size_t yPos = dragAreaHeight;
	// Update children in case they are dettached from parent
	for (size_t i = 0; i < children.size(); ++i) {
		bool isAttached = false;
		auto lock = children[i].lock();
		if (!lock) {
			children.erase(children.begin() + i);
			--i;
			continue;
		}
		if (auto parent = lock->GetParent(); !parent.has_value() || parent.value().lock().get() != this) {
			children.erase(children.begin() + i);
			--i;
			continue;
		}
		lock->SetLocalPosition(0, yPos);
		yPos += lock->GetHeight();
	}
}

void Demo::IContainer::Draw(unsigned long long deltaTime)
{
	IDraggable::Draw(deltaTime);
	graphicsDevice->DrawRectangle(
		*camera,
		GetWorldX(), 
		GetWorldY() + dragAreaHeight, 
		(std::max)(dragAreaWidth, GetMaxWidthOfChildren()), 
		isHovered ? 2 * dragAreaHeight + GetHeightOfChildren() : dragAreaHeight + GetHeightOfChildren(),
		0xFF888888, 
		true
	);
}
