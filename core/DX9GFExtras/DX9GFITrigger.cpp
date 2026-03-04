#include "DX9GFITrigger.h"
#include "../DX9GFInputManager.h"

void DX9GF::ITrigger::Update(unsigned long long deltaTime)
{
	if (IsHovering(deltaTime)) {
		this->onHover(this);
	}
	if (IsClicked(deltaTime)) {
		this->onClick(this);
	}
	if (IsHeld(deltaTime)) {
		this->onHeld(this);
	}
}

bool DX9GF::ITrigger::IsClicked(unsigned long long deltaTime)
{
	auto input = DX9GF::InputManager::GetInstance();
	return IsHovering(deltaTime)
		&& input->MouseDown(InputManager::MouseButton::Left);
}

bool DX9GF::ITrigger::IsHeld(unsigned long long deltaTime)
{
	auto input = DX9GF::InputManager::GetInstance();
	if (isHeld && input->MouseUp(InputManager::MouseButton::Left)) {
		return isHeld = false;
	}
	if (!isHeld && IsHovering(deltaTime) && input->MouseDown(InputManager::MouseButton::Left)) {
		return isHeld = true;
	}
	return isHeld;
}

void DX9GF::ITrigger::SetOnHover(std::function<void(ITrigger*)> onHover)
{
	this->onHover = onHover;
}

void DX9GF::ITrigger::SetOnClick(std::function<void(ITrigger*)> onClick)
{
	this->onClick = onClick;
}

void DX9GF::ITrigger::SetOnHeld(std::function<void(ITrigger*)> onHeld)
{
	this->onHeld = onHeld;
}
