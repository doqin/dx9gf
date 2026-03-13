#include "Ellipse.h"

void GO::Ellipse::Update(unsigned long long deltaTime)
{
	trigger->Update(deltaTime);
}

std::weak_ptr<DX9GF::EllipseCollider> GO::Ellipse::GetCollider()
{
	return collider;
}

void GO::Ellipse::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::shared_ptr<DX9GF::ColliderManager> colliderManager)
{
    this->graphicsDevice = graphicsDevice;
    this->camera = camera;
    collider = std::make_shared<DX9GF::EllipseCollider>(
        transformManager,
        shared_from_this(),
        width,
        height,
        0,
        0
    );
    collider->SetOriginCenter();
    colliderManager->Add(collider);
    trigger = std::make_shared<DX9GF::EllipseTrigger>(
        transformManager,
        shared_from_this(),
        width,
        height,
        0,
        0
    );
    trigger->SetOriginCenter();
    trigger->Init(camera);
    trigger->SetOnHeldRight([](DX9GF::ITrigger* thisObject) {
        auto parent = dynamic_pointer_cast<Ellipse>(thisObject->GetParent().value().lock());
        auto input = DX9GF::InputManager::GetInstance();
        auto deltaX = input->GetRelativeMouseX();
        parent->SetLocalRotation(parent->GetLocalRotation() + deltaX * 0.1f);
        });
    trigger->SetOnHeldLeft([colliderManager](DX9GF::ITrigger* thisObject) {
        auto parent = dynamic_pointer_cast<Ellipse>(thisObject->GetParent().value().lock());
        auto input = DX9GF::InputManager::GetInstance();
        auto deltaX = input->GetRelativeMouseX();
        auto deltaY = input->GetRelativeMouseY();
        auto localX = parent->GetLocalX();
        auto localY = parent->GetLocalY();
        auto parentCollider = parent->GetCollider().lock();
        float nextWorldX = parentCollider->GetWorldX() + deltaX;
        float nextWorldY = parentCollider->GetWorldY() + deltaY;
        auto safePos = colliderManager->GetSafePosition(parentCollider, nextWorldX, nextWorldY);
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

void GO::Ellipse::Draw(unsigned long long deltaTime)
{
	graphicsDevice->DrawEllipse(
		*camera, 
		GetWorldX(), 
		GetWorldY(), 
		width, 
		height, 
		GetWorldRotation(),
		GetWorldScaleX(),
		GetWorldScaleY(),
		width / 2,
		height / 2,
		0xFF00FF00, 
		trigger->IsHeld(deltaTime)
	);
	graphicsDevice->DrawEllipse(
		*camera, 
		trigger->GetWorldX(), 
		trigger->GetWorldY(), 
		width, 
		height, 
		trigger->GetWorldRotation(), 
		trigger->GetWorldScaleX(), 
		trigger->GetWorldScaleY(), 
		trigger->GetOriginX(), 
		trigger->GetOriginY(), 
		0x550000FF,
		false
	);
}

void GO::Ellipse::Dispose()
{
}
