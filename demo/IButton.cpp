#include "pch.h"
#include "IButton.h"

void Demo::IButton::SetOnReleaseLeft(std::function<void(DX9GF::ITrigger*)> cb)
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

