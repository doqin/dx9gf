#include "Ellipse.h"

void GO::Ellipse::Update(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

void GO::Ellipse::Init()
{
	trigger = std::make_shared<DX9GF::EllipseTrigger>(
		transformManager, 
		shared_from_this(), 
		width, 
		height,
		0, 
		0
	);
	trigger->SetOnHeld([](DX9GF::ITrigger* thisObject) {
		if (auto parent = thisObject->GetParent().value().lock()) {
			auto input = DX9GF::InputManager::GetInstance();
			auto deltaX = input->GetRelativeMouseX();
			auto deltaY = input->GetRelativeMouseY();
			auto localX = parent->GetLocalX();
			auto localY = parent->GetLocalY();
			auto newX = localX + deltaX;
			auto newY = localY + deltaY;
			parent->SetLocalPosition(
				newX,
				newY
			);
		}
		else {
			throw std::runtime_error("lock failed");
		}
	});
}

void GO::Ellipse::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawEllipse(GetWorldX() + width / 2, GetWorldY() + height / 2, width, height, 0xFF00FF00, trigger->IsHeld(deltaTime));
	graphicsDevice->DrawEllipse(trigger->GetWorldX() + width / 2, trigger->GetWorldY() + height / 2, width, height, 0x550000FF, false);
}

void GO::Ellipse::Dispose()
{
}
