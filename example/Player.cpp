#include "Player.h"
#include "resource.h"
#include <memory>

GO::Player::~Player()
{
	auto it = std::find(worldColliders->begin(), worldColliders->end(), collider);
	worldColliders->erase(it);
}

void GO::Player::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::CommandBuffer* commandBuffer, DX9GF::Camera* camera, std::vector<std::shared_ptr<DX9GF::ICollider>>* worldColliders)
{
	this->graphicsDevice = graphicsDevice;
	this->commandBuffer = commandBuffer;
	this->camera = camera;
	marioTexture = std::make_shared<DX9GF::Texture>(this->graphicsDevice);
	marioTexture->LoadTexture(IDB_PNG3, 156, 7497);
	mario = std::make_shared<DX9GF::AnimatedSprite>(marioTexture.get(), DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3), 12);
	mario->SetOrigin(17.0f, 51.0f / 3.0f);
	mario->SetScale(2, 2);
	collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, shared_from_this(), 39, 51, 0, 0);
	collider->SetOriginCenter();
	worldColliders->push_back(collider);
	this->worldColliders = worldColliders;
}

void GO::Player::Update(unsigned long long deltaTime)
{
	if (GetState() == State::Active) {
		auto inputManager = DX9GF::InputManager::GetInstance();
		float xDir = 0;
		float yDir = 0;
		const float velocity = 200;
		if (inputManager->KeyPress(DIK_A)) xDir -= 1;
		if (inputManager->KeyPress(DIK_D)) xDir += 1;
		if (inputManager->KeyPress(DIK_W)) yDir -= 1;
		if (inputManager->KeyPress(DIK_S)) yDir += 1;
		if (inputManager->KeyPress(DIK_E)) SetLocalRotation(GetLocalRotation() + 25.f * deltaTime / 1000);
		if (inputManager->KeyPress(DIK_Q)) SetLocalRotation(GetLocalRotation() - 25.f * deltaTime / 1000);
		if (inputManager->KeyPress(DIK_X)) commandBuffer->PushCommand(std::make_shared<DX9GF::DestroyObjectCommand>(shared_from_this(), [this]() {}));
		auto dX = xDir * velocity * deltaTime / 1000;
		auto dY = yDir * velocity * deltaTime / 1000;
		for (auto& otherCollider : *worldColliders) {
			if (otherCollider == collider) continue;
			auto colliderWorldX = collider->GetWorldX();
			auto colliderWorldY = collider->GetWorldY();
			// handle x and y per axis to prevent getting stuck instead of sliding
			if (auto pos = collider->IsIntersecting(otherCollider, colliderWorldX + dX, colliderWorldY); pos.has_value()) {
				auto& [correctedX, correctedY] = pos.value();
				dX = correctedX - colliderWorldX;
			}
			if (auto pos = collider->IsIntersecting(otherCollider, colliderWorldX, colliderWorldY + dY); pos.has_value()) {
				auto& [correctedX, correctedY] = pos.value();
				dY = correctedY - colliderWorldY;
			}
		}
		SetLocalPosition(
			GetLocalX() + dX,
			GetLocalY() + dY
		);
	}
}

void GO::Player::Draw(unsigned long long deltaTime)
{
	if (GetState() == State::Active) {
		auto worldX = GetWorldX();
		auto worldY = GetWorldY();
		auto worldRotation = GetWorldRotation();
		mario->SetPosition(worldX, worldY);
		mario->SetRotation(worldRotation);
		mario->Begin();
		mario->Draw(*camera, deltaTime);
		mario->End();
		this->graphicsDevice->DrawRectangle(
			*camera,
			collider->GetWorldX(),
			collider->GetWorldY(),
			collider->GetWidth(),
			collider->GetHeight(),
			collider->GetWorldRotation(),
			collider->GetWorldScaleX(),
			collider->GetWorldScaleY(),
			collider->GetOriginX(),
			collider->GetOriginY(),
			0xFF00FF00,
			false
		);
	}
}
