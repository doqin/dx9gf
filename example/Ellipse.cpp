#include "Ellipse.h"

void GO::Ellipse::MainUpdate(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

void GO::Ellipse::Init()
{
	trigger = std::make_shared<DX9GF::EllipseTrigger>(shared_from_this(), -width / 2, -height / 2, width, height);
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

void GO::Ellipse::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawEllipse(absoluteX, absoluteY, width, height, 0xFF00FF00, trigger->IsHeld(deltaTime));
}

void GO::Ellipse::Dispose()
{
}
