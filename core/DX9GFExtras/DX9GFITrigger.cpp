#include "DX9GFITrigger.h"
#include "../DX9GFInputManager.h"

namespace {
	bool IsClickedButton(DX9GF::ITrigger* trigger, unsigned long long deltaTime, DX9GF::InputManager::MouseButton button)
	{
		auto input = DX9GF::InputManager::GetInstance();
		return trigger->IsHovering(deltaTime)
			&& input->MouseDown(button);
	}

	bool IsHeldButton(DX9GF::ITrigger* trigger, unsigned long long deltaTime, DX9GF::InputManager::MouseButton button, bool& isHeld)
	{
		auto input = DX9GF::InputManager::GetInstance();
		if (isHeld && input->MouseUp(button)) {
			isHeld = false;
			return false;
		}
		if (!isHeld && trigger->IsHovering(deltaTime) && input->MouseDown(button)) {
			isHeld = true;
			return true;
		}
		return isHeld;
	}
}

void DX9GF::ITrigger::Update(unsigned long long deltaTime)
{
	if (IsHovering(deltaTime)) {
		this->onHover(this);
	}
	if (IsClickedLeft(deltaTime)) {
		this->onClickLeft(this);
	}
	if (IsClickedRight(deltaTime)) {
		this->onClickRight(this);
	}
	if (IsHeldLeft(deltaTime)) {
		this->onHeldLeft(this);
	}
	if (IsHeldRight(deltaTime)) {
		this->onHeldRight(this);
	}
}

void DX9GF::ITrigger::Init(Camera* camera)
{
	this->camera = camera;
}

bool DX9GF::ITrigger::IsClicked(unsigned long long deltaTime)
{
	return IsClickedLeft(deltaTime);
}

bool DX9GF::ITrigger::IsClickedLeft(unsigned long long deltaTime)
{
	return IsClickedButton(this, deltaTime, InputManager::MouseButton::Left);
}

bool DX9GF::ITrigger::IsClickedRight(unsigned long long deltaTime)
{
	return IsClickedButton(this, deltaTime, InputManager::MouseButton::Right);
}

bool DX9GF::ITrigger::IsHeld(unsigned long long deltaTime)
{
	return IsHeldLeft(deltaTime);
}

bool DX9GF::ITrigger::IsHeldLeft(unsigned long long deltaTime)
{
	return IsHeldButton(this, deltaTime, InputManager::MouseButton::Left, isHeldLeft);
}

bool DX9GF::ITrigger::IsHeldRight(unsigned long long deltaTime)
{
	return IsHeldButton(this, deltaTime, InputManager::MouseButton::Right, isHeldRight);
}

void DX9GF::ITrigger::SetOnHover(std::function<void(ITrigger*)> onHover)
{
	this->onHover = onHover;
}

void DX9GF::ITrigger::SetOnClickLeft(std::function<void(ITrigger*)> onClick)
{
	this->onClickLeft = onClick;
}

void DX9GF::ITrigger::SetOnClickRight(std::function<void(ITrigger*)> onClick)
{
	this->onClickRight = onClick;
}

void DX9GF::ITrigger::SetOnHeldLeft(std::function<void(ITrigger*)> onHeld)
{
	this->onHeldLeft = onHeld;
}

void DX9GF::ITrigger::SetOnHeldRight(std::function<void(ITrigger*)> onHeld)
{
	this->onHeldRight = onHeld;
}
bool DX9GF::ITrigger::drawTrigger = false;