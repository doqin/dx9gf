#include "Ellipse.h"

void GO::Ellipse::Update(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

std::weak_ptr<DX9GF::EllipseCollider> GO::Ellipse::GetCollider()
{
	return collider;
}

void GO::Ellipse::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::weak_ptr<DX9GF::ICollider>>* worldColliders)
{
	this->graphicsDevice = graphicsDevice;
	this->camera = camera;
	collider = std::make_shared<DX9GF::EllipseCollider>(
		transformManager, 
		shared_from_this(), 
		width, 
		height, 
		-width / 2, 
		-height / 2
	);
	worldColliders->push_back(collider);
	trigger = std::make_shared<DX9GF::EllipseTrigger>(
		transformManager, 
		shared_from_this(), 
		width, 
		height,
		-width / 2, 
		-height / 2
	);
	trigger->Init(camera);
	trigger->SetOnHeld([worldColliders](DX9GF::ITrigger* thisObject) {
		auto parent = dynamic_pointer_cast<Ellipse>(thisObject->GetParent().value().lock());
		auto input = DX9GF::InputManager::GetInstance();
		auto deltaX = input->GetRelativeMouseX();
		auto deltaY = input->GetRelativeMouseY();
		auto localX = parent->GetLocalX();
		auto localY = parent->GetLocalY();
		auto parentCollider = parent->GetCollider().lock();
		for (auto& collider : *worldColliders) {
			if (auto ellipseCollider = dynamic_pointer_cast<DX9GF::EllipseCollider>(collider.lock());
				ellipseCollider == parentCollider) continue; // skip if it's our collider
			if (auto pos = parentCollider->IsIntersecting(
				collider,
				parentCollider->GetWorldX() + deltaX,
				parentCollider->GetWorldY() + deltaY); pos.has_value()) {
				auto& [posX, posY] = pos.value();
				deltaX = posX - parentCollider->GetWorldX();
				deltaY = posY - parentCollider->GetWorldY();
			}
		}
		parent->SetLocalPosition(
			localX + deltaX,
			localY + deltaY
		);
	});
}

void GO::Ellipse::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawEllipse(*camera, GetWorldX(), GetWorldY(), width, height, 0xFF00FF00, trigger->IsHeld(deltaTime));
	graphicsDevice->DrawEllipse(*camera, trigger->GetWorldX() + width / 2, trigger->GetWorldY() + height / 2, width, height, 0x550000FF, false);
}

void GO::Ellipse::Dispose()
{
}
