#include "Player.h"

void GO::Player::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::weak_ptr<DX9GF::ICollider>>* worldColliders)
{
	this->graphicsDevice = graphicsDevice;
	this->camera = camera;
	mario = std::make_shared<DX9GF::AnimatedSprite>(this->graphicsDevice);
	mario->LoadSpriteSheet(
		L".\\Resources\\spritesheet.png",
		DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3),
		156,
		7497,
		12
	);
	collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, shared_from_this(), 39, 51, -39 / 2, -51 / 2);
	worldColliders->push_back(collider);
	this->worldColliders = worldColliders;
}

void GO::Player::Update(unsigned long long deltaTime)
{
	auto inputManager = DX9GF::InputManager::GetInstance();
	float xDir = 0;
	float yDir = 0;
	const float velocity = 200;
	if (inputManager->KeyPress(DIK_A)) xDir -= 1;
	if (inputManager->KeyPress(DIK_D)) xDir += 1;
	if (inputManager->KeyPress(DIK_W)) yDir -= 1;
	if (inputManager->KeyPress(DIK_S)) yDir += 1;
	auto dX = xDir * velocity * deltaTime / 1000;
	auto dY = yDir * velocity * deltaTime / 1000;
	for (auto& otherCollider : *worldColliders) {
		if (otherCollider.lock() == collider) continue;
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

void GO::Player::Draw(unsigned long long deltaTime)
{
	auto worldX = GetWorldX();
	auto worldY = GetWorldY();
	mario->SetPosition(worldX - 39 / 2, worldY - 51 / 2);
	mario->Begin();
	mario->Draw(*camera, deltaTime);
	mario->End();
	this->graphicsDevice->DrawRectangle(*camera, collider->GetWorldX(), collider->GetWorldY(), collider->GetWidth(), collider->GetHeight(), 0xFF00FF00, false);
}
