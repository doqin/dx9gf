#include "pch.h"
#include "IButton.h"

Demo::IButton::IButton(std::shared_ptr<DX9GF::TransformManager> tm, float x, float y, float w, float h)
	: IGameObject(tm, x, y)
{
	this->width = w;
	this->height = h;
	this->currentState = ButtonState::IDLE;
}

void Demo::IButton::SetOnClicked(std::function<void(DX9GF::ITrigger*)> cb)
{
	//save the button's feature
	this->callback = cb;

	if (this->trigger && this->callback)
	{
		this->trigger->SetOnReleaseLeft(this->callback);
	}
}

//check if the cursor is hovering over the button.
void Demo::IButton::Update(unsigned long long deltaTime)
{
	if (!this->trigger)
		return;

	//update trigger
	this->trigger->Update(deltaTime);

	//change button state
	if (this->trigger->IsHeldLeft(deltaTime)) {
		this->currentState = ButtonState::CLICKED;
	}
		
	else if (this->trigger->IsHovering(deltaTime))
		this->currentState = ButtonState::HOVER;
	else
		this->currentState = ButtonState::IDLE;
}

std::shared_ptr<DX9GF::RectangleTrigger> Demo::IButton::GetTrigger() { return this->trigger; }

float Demo::IButton::GetWidth() const { return this->width; }

float Demo::IButton::GetHeight() const { return this->height; }
