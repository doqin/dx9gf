#include "Rectangle.h"

void GO::Rectangle::Init()
{
	trigger = std::make_shared<DX9GF::RectangleTrigger>(
		shared_from_this(),
		-width / 2,
		-height / 2,
		width,
		height
	);
	trigger->SetOnHeld([](DX9GF::ITrigger* thisObject) {
			if (auto parent = thisObject->GetParent().value().lock()) {
				auto input = DX9GF::InputManager::GetInstance();
				{
					std::scoped_lock lock(parent->GetMutex());
					parent->SetRelativePosition(
						parent->GetRelativeX() + input->GetRelativeMouseX(),
						parent->GetRelativeY() + input->GetRelativeMouseY()
					);
				}
			}
		}
	);
}

void GO::Rectangle::MainUpdate(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

void GO::Rectangle::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawRectangle(absoluteX - width / 2, absoluteY - height / 2, width, height, 0xFFFF0000, trigger->IsHeld(deltaTime));
}

void GO::Rectangle::Dispose()
{
}
