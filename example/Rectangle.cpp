#include "Rectangle.h"

void GO::Rectangle::Init()
{
	trigger = std::make_shared<DX9GF::RectangleTrigger>(
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

void GO::Rectangle::Update(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

void GO::Rectangle::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawRectangle(GetWorldX() - width / 2, GetWorldY() - height / 2, width, height, 0xFFFF0000, trigger->IsHeld(deltaTime));
	graphicsDevice->DrawRectangle(trigger->GetWorldX(), trigger->GetWorldY(), width, height, 0x550000FF, false);
}

void GO::Rectangle::Dispose()
{
}
