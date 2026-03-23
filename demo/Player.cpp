#include "pch.h"
#include "Player.h"
#include "resource.h"

Demo::Player::~Player() {
	colliderManager->Remove(collider);
}

void Demo::Player::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::ColliderManager* colliderManager, DX9GF::Camera* camera) {
	// External components
	this->graphicsDevice = graphicsDevice;
	this->colliderManager = colliderManager;
	this->camera = camera;
	// Create texture
	spritesheet = std::make_shared<DX9GF::Texture>(graphicsDevice);
	spritesheet->LoadTexture(IDB_PNG1);
	// Create sprites
	idleDown = std::make_shared<DX9GF::StaticSprite>(spritesheet.get());
	idleDown->SetSrcRect({.left = 0, .top = 0, .right = 32, .bottom = 32});
	idleUp = std::make_shared<DX9GF::StaticSprite>(spritesheet.get());
	idleUp->SetSrcRect({.left = 0, .top = 32, .right = 32, .bottom = 64});
	idleRight = std::make_shared<DX9GF::StaticSprite>(spritesheet.get());
	idleRight->SetSrcRect({.left = 0, .top = 64, .right = 32, .bottom = 96});
	idleLeft = std::make_shared<DX9GF::StaticSprite>(spritesheet.get());
	idleLeft->SetSrcRect({.left = 0, .top = 96, .right = 32, .bottom = 128});
	walkingDown = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateFrames(128, 128, 32, 32, 4, 0));
	walkingUp = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateFrames(128, 128, 32, 32, 4, 4));
	walkingRight = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateFrames(128, 128, 32, 32, 4, 8));
	walkingLeft = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateFrames(128, 128, 32, 32, 4, 12));
	// Align sprites
	idleDown->SetOrigin(16, 16);
	idleUp->SetOrigin(16, 16);
	idleRight->SetOrigin(16, 16);
	idleLeft->SetOrigin(16, 16);
	walkingDown->SetOrigin(16, 16);
	walkingUp->SetOrigin(16, 16);
	walkingRight->SetOrigin(16, 16);
	walkingLeft->SetOrigin(16, 16);
	// Set framerate
	walkingDown->SetFrameRate(12);
	walkingUp->SetFrameRate(12);
	walkingRight->SetFrameRate(12);
	walkingLeft->SetFrameRate(12);
	// Create collider
	collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, shared_from_this(), 8, 4, 0, 14);
	collider->SetOriginCenter();
	this->colliderManager->Add(collider);
}

void Demo::Player::Update(unsigned long long deltaTime) {
	auto inpMan = DX9GF::InputManager::GetInstance();
	// Movement
	D3DXVECTOR2 dir{0, 0};
	if (inpMan->KeyPress(DIK_D)) dir.x += 1;
	if (inpMan->KeyPress(DIK_A)) dir.x -= 1;
	if (inpMan->KeyPress(DIK_S)) dir.y += 1;
	if (inpMan->KeyPress(DIK_W)) dir.y -= 1;
	if (dir.x == 1) state = State::Right;
	if (dir.x == -1) state = State::Left;
	if (dir.y == 1) state = State::Down;
	if (dir.y == -1) state = State::Up;
	D3DXVECTOR2 dirNorm;
	D3DXVec2Normalize(&dirNorm, &dir);
	bool isRunning = false;
	if (inpMan->KeyPress(DIK_LSHIFT)) {
		isRunning = true;
		dirNorm.x *= SPRINT_MULTIPLIER;
		dirNorm.y *= SPRINT_MULTIPLIER;
		walkingDown->SetFrameRate(12 * SPRINT_MULTIPLIER);
		walkingUp->SetFrameRate(12 * SPRINT_MULTIPLIER);
		walkingRight->SetFrameRate(12 * SPRINT_MULTIPLIER);
		walkingLeft->SetFrameRate(12 * SPRINT_MULTIPLIER);
	}
	else {
		walkingDown->SetFrameRate(12);
		walkingUp->SetFrameRate(12);
		walkingRight->SetFrameRate(12);
		walkingLeft->SetFrameRate(12);
	}
	if (dirNorm.x != 0 || dirNorm.y != 0) isWalking = true;
	else {
		isWalking = false;
		walkingDown->SetFrame(0);
		walkingUp->SetFrame(0);
		walkingRight->SetFrame(0);
		walkingLeft->SetFrame(0);
	}
	// if (dirNorm.x == 0 && dirNorm.y == 0) return;
	float dX = dirNorm.x * VELOCITY * deltaTime / 1000.f;
	float dY = dirNorm.y * VELOCITY * deltaTime / 1000.f;
	auto [finalDX, finalDY] = colliderManager->GetSlidingDeltas(collider, dX, dY);
	auto [currentX, currentY] = GetLocalPosition();
	SetLocalPosition(currentX + finalDX, currentY + finalDY);
	// Camera movement
	if (followCamera) {
		auto cameraPos = camera->GetPosition();
		auto [playerPosX, playerPosY] = GetLocalPosition();
		D3DXVECTOR2 vec{ playerPosX - cameraPos.x, playerPosY - cameraPos.y };
		// Update only if there is difference in distance
		if (vec.x != 0 || vec.y != 0) {
			if (std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2)) < CAMERA_SNAP_MARGIN) {
				camera->SetPosition(playerPosX, playerPosY);
			}
			else {
				D3DXVECTOR2 vecNorm;
				D3DXVec2Normalize(&vecNorm, &vec);
				float speedX = vecNorm.x * (std::min)(isRunning ? VELOCITY * SPRINT_MULTIPLIER : VELOCITY, (CAMERA_VELOCITY + CAMERA_ACCELERATION * cameraDeltaTime / 1000.f)) * deltaTime / 1000.f;
				float speedY = vecNorm.y * (std::min)(isRunning ? VELOCITY * SPRINT_MULTIPLIER : VELOCITY, (CAMERA_VELOCITY + CAMERA_ACCELERATION * cameraDeltaTime / 1000.f)) * deltaTime / 1000.f;
				camera->SetPosition(cameraPos.x + speedX, cameraPos.y + speedY);
			}
			cameraDeltaTime += deltaTime;
			if ((CAMERA_VELOCITY + CAMERA_ACCELERATION * cameraDeltaTime / 1000.f) > (isRunning ? VELOCITY * SPRINT_MULTIPLIER : VELOCITY)) {
				cameraDeltaTime = ((isRunning ? VELOCITY * SPRINT_MULTIPLIER : VELOCITY) - CAMERA_VELOCITY) / CAMERA_ACCELERATION * 1000.f;
			}
		}
		else {

			cameraDeltaTime = (std::max)(0.f, cameraDeltaTime - deltaTime);
		}
	}
	if (isInvincible) {
		if (timeSinceTurnedInvincible > INVINCIBILITY_DURATION) {
			isInvincible = false;
		}
		else {
			timeSinceTurnedInvincible += deltaTime / 1000.f;
		}
	}
}

void Demo::Player::Draw(unsigned long long deltaTime) {
	if (!isInvincible || static_cast<int>(timeSinceTurnedInvincible / BLINKING_DURATION) % 2) {
		switch (state) {
		case State::Down: {
			if (isWalking) {
				walkingDown->Begin();
				auto [x, y] = GetWorldPosition();
				walkingDown->SetPosition(x, y);
				walkingDown->Draw(*camera, deltaTime);
				walkingDown->End();
			}
			else {
				idleDown->Begin();
				auto [x, y] = GetWorldPosition();
				idleDown->SetPosition(x, y);
				idleDown->Draw(*camera, deltaTime);
				idleDown->End();
			}
		}
		break;
		case State::Up: {
			if (isWalking) {
				walkingUp->Begin();
				auto [x, y] = GetWorldPosition();
				walkingUp->SetPosition(x, y);
				walkingUp->Draw(*camera, deltaTime);
				walkingUp->End();
			}
			else {
				idleUp->Begin();
				auto [x, y] = GetWorldPosition();
				idleUp->SetPosition(x, y);
				idleUp->Draw(*camera, deltaTime);
				idleUp->End();
			}
		}
		break;
		case State::Right: {
			if (isWalking) {
				walkingRight->Begin();
				auto [x, y] = GetWorldPosition();
				walkingRight->SetPosition(x, y);
				walkingRight->Draw(*camera, deltaTime);
				walkingRight->End();
			}
			else {
				idleRight->Begin();
				auto [x, y] = GetWorldPosition();
				idleRight->SetPosition(x, y);
				idleRight->Draw(*camera, deltaTime);
				idleRight->End();
			}
		}
		break;
		case State::Left: {
			if (isWalking) {
				walkingLeft->Begin();
				auto [x, y] = GetWorldPosition();
				walkingLeft->SetPosition(x, y);
				walkingLeft->Draw(*camera, deltaTime);
				walkingLeft->End();
			}
			else {
				idleLeft->Begin();
				auto [x, y] = GetWorldPosition();
				idleLeft->SetPosition(x, y);
				idleLeft->Draw(*camera, deltaTime);
				idleLeft->End();
			}
		}
		break;
		default:
			break;
		}
	}
	collider->Draw(graphicsDevice, *camera);
}

void Demo::Player::SetFollowCamera(bool followCamera)
{
	this->followCamera = followCamera;
}

float Demo::Player::GetVelocity() const
{
	return VELOCITY;
}

float Demo::Player::SetVelocity(float velocity)
{
	return this->VELOCITY = velocity;
}

bool Demo::Player::TakeDamage(float damage)
{
	if (isInvincible) return IsDead();
	health -= damage;
	isInvincible = true;
	timeSinceTurnedInvincible = 0.f;
	if (health < 0) health = 0;
	return IsDead();
}

bool Demo::Player::IsDead() const
{
	return health <= 0;
}

void Demo::Player::SetHealth(float hp)
{
	health = hp;
}

float Demo::Player::GetMaxHealth() const
{
	return MAX_HEALTH;
}

float Demo::Player::GetHealth() const
{
	return health;
}

std::weak_ptr<DX9GF::RectangleCollider> Demo::Player::GetCollider()
{
	return collider;
}
