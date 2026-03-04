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
		-width / 2, 
		-height / 2
	);
	trigger->SetOnHeld([](DX9GF::ITrigger* thisObject) {
		if (auto parent = thisObject->GetParent().value().lock()) {
			auto input = DX9GF::InputManager::GetInstance();
			parent->SetLocalPosition(
				parent->GetLocalX() + input->GetRelativeMouseX(),
				parent->GetLocalY() + input->GetRelativeMouseY()
			);
		}
	});
}

void GO::Ellipse::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawEllipse(GetWorldX(), GetWorldY(), width, height, 0xFF00FF00, trigger->IsHeld(deltaTime));
	graphicsDevice->DrawEllipse(trigger->GetWorldX() + width / 2, trigger->GetWorldY() + height / 2, width, height, 0x550000FF, false);
}

void GO::Ellipse::Dispose()
{
}
