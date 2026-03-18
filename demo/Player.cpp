#include "pch.h"
#include "Player.h"
#include "resource.h"

Demo::Player::~Player() {
	colliderManager->Remove(collider);
}

void Demo::Player::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::ColliderManager* colliderManager, DX9GF::Camera* camera) {
	this->graphicsDevice = graphicsDevice;
	this->colliderManager = colliderManager;
	this->camera = camera;
	spritesheet = std::make_shared<DX9GF::Texture>(graphicsDevice);
	spritesheet->LoadTexture(IDB_PNG1);
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
	collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, shared_from_this(), 8, 4, 0, 14);
	collider->SetOriginCenter();
	this->colliderManager->Add(collider);
}

void Demo::Player::Update(unsigned long long deltaTime) {
	auto inpMan = DX9GF::InputManager::GetInstance();
	float xDir = 0;
	float yDir = 0;
	if (inpMan->KeyPress(DIK_D)) xDir += 1;
	if (inpMan->KeyPress(DIK_A)) xDir -= 1;
	if (inpMan->KeyPress(DIK_S)) yDir += 1;
	if (inpMan->KeyPress(DIK_W)) yDir -= 1;
	if (xDir == 1) state = State::Right;
	if (xDir == -1) state = State::Left;
	if (yDir == 1) state = State::Down;
	if (yDir == -1) state = State::Up;
	if (inpMan->KeyPress(DIK_LSHIFT)) {
		xDir *= SPRINT_MULTIPLIER;
		yDir *= SPRINT_MULTIPLIER;
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
	if (xDir != 0 || yDir != 0) isWalking = true;
	else {
		isWalking = false;
		walkingDown->SetFrame(0);
		walkingUp->SetFrame(0);
		walkingRight->SetFrame(0);
		walkingLeft->SetFrame(0);
	}
	if (xDir == 0 && yDir == 0) return;
	float dX = xDir * VELOCITY * deltaTime / (float)1000;
	float dY = yDir * VELOCITY * deltaTime / (float)1000;
	auto [finalDX, finalDY] = colliderManager->GetSlidingDeltas(collider, dX, dY);
	auto [currentX, currentY] = GetLocalPosition();
	SetLocalPosition(currentX + finalDX, currentY + finalDY);
}

void Demo::Player::Draw(unsigned long long deltaTime) {
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
	collider->Draw(graphicsDevice, camera);
}