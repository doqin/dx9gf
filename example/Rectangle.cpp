#include "pch.h"
#include "Rectangle.h"

std::weak_ptr<DX9GF::RectangleCollider> GO::Rectangle::GetCollider()
{
	return collider;
}

void GO::Rectangle::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::shared_ptr<DX9GF::ColliderManager> colliderManager)
{
	this->camera = camera;
	this->graphicsDevice = graphicsDevice;
	collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, shared_from_this(), width, height, 0, 0);
	collider->SetOriginCenter();
	colliderManager->Add(collider);
	trigger = std::make_shared<DX9GF::RectangleTrigger>(transformManager, shared_from_this(), width, height, 0, 0);
	trigger->SetOriginCenter();
	trigger->Init(camera);
	trigger->SetOnHeldRight([](DX9GF::ITrigger* thisObject) {
		auto parent = dynamic_pointer_cast<Rectangle>(thisObject->GetParent().value().lock());
		auto input = DX9GF::InputManager::GetInstance();
		auto deltaX = input->GetRelativeMouseX();
		parent->SetLocalRotation(parent->GetLocalRotation() + deltaX * 0.1f);
		});
	trigger->SetOnHeldLeft([colliderManager](DX9GF::ITrigger* thisObject) {
		auto parent = dynamic_pointer_cast<Rectangle>(thisObject->GetParent().value().lock());
		auto input = DX9GF::InputManager::GetInstance();
		auto deltaX = input->GetRelativeMouseX();
		auto deltaY = input->GetRelativeMouseY();
		auto localX = parent->GetLocalX();
		auto localY = parent->GetLocalY();
		auto parentCollider = parent->GetCollider().lock();
		float targetWorldX = parentCollider->GetWorldX() + deltaX;
		float targetWorldY = parentCollider->GetWorldY() + deltaY;
		auto safePos = colliderManager->GetSafePosition(parentCollider, targetWorldX, targetWorldY);
		if (safePos.has_value()) {
			auto& [posX, posY] = safePos.value();
			deltaX = posX - parentCollider->GetWorldX();
			deltaY = posY - parentCollider->GetWorldY();
		}
		parent->SetLocalPosition(
			localX + deltaX,
			localY + deltaY
		);
	});
}

void GO::Rectangle::Update(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

void GO::Rectangle::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawRectangle(*camera, GetWorldX(), GetWorldY(), width, height, GetWorldRotation(), GetWorldScaleX(), GetWorldScaleY(), width / 2, height / 2, 0xFFFF0000, trigger->IsHeld(deltaTime));
	trigger->Draw(graphicsDevice, camera);
	//graphicsDevice->DrawRectangle(*camera, trigger->GetWorldX(), trigger->GetWorldY(), trigger->GetWidth(), trigger->GetHeight(), trigger->GetWorldRotation(), trigger->GetWorldScaleX(), trigger->GetWorldScaleY(), trigger->GetOriginX(), trigger->GetOriginY(), 0x550000FF, false);
	collider->Draw(graphicsDevice, camera);
	//graphicsDevice->DrawRectangle(*camera, collider->GetWorldX(), collider->GetWorldY(), collider->GetWidth(), collider->GetHeight(), collider->GetWorldRotation(), collider->GetWorldScaleX(), collider->GetWorldScaleY(), collider->GetOriginX(), collider->GetOriginY(), 0x5500FF00, false);
}

void GO::Rectangle::Dispose()
{
}
