#include "pch.h"
#include "DeadlineEnemy.h"
#include "resource.h"
#include "RNG.h"
#include "PopUpMessage.h"
#include <algorithm>

void Demo::DeadlineEnemy::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera) {
	texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	texture->LoadTexture(L"assets/placeholder.png"); // TODO: change when real asset is available
	sprite = std::make_shared<DX9GF::AnimatedSprite>(texture.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 64, 64, 12), 12);
	sprite->SetOrigin(32, 32);
	sprite->SetScale(2.f);

	projTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	projTexture->LoadTexture(L"assets/placeholder-round-projectile.png"); // TODO: change when real asset is available

	SetGoldReward(static_cast<int>(std::round(GetMaxHealth())));
	InitCardSpawnTrigger(camera, 128.f, 128.f);
}

void Demo::DeadlineEnemy::Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime) {
	if (sprite) {
		sprite->Begin();
		auto [x, y] = GetWorldPosition();
		sprite->SetPosition(x, y);
		sprite->Draw(*camera, deltaTime);
		sprite->End();
	}
	IEnemy::Draw(graphicsDevice, camera, deltaTime);
}

void Demo::DeadlineEnemy::OnTurnBegin(std::shared_ptr<Player> player, std::shared_ptr<PopUpMessage> popUpMessage, int currentTurn) {
	this->player = player;

	if (abilityCooldown > 0) {
		--abilityCooldown;
		return;
	}

	if (auto lock = this->player.lock()) {
		lock->AddModifier(ModifierType::EnergyDrain, 2, 1.0f, false, 0);
	}

	if (popUpMessage) {
		popUpMessage->ShowMessage(L"Deadline crunch drains your Energy!");
	}

	abilityCooldown = 3;
}

void Demo::DeadlineEnemy::StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies, std::shared_ptr<PopUpMessage> popUpMessage, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, int currentTurn) {
	(void)enemies; (void)popUpMessage; (void)graphicsDevice; (void)camera; (void)currentTurn;
	this->player = player;

	const float baseDamage = 6.f;
	const float finalDamage = CalculateOutgoingDamage(baseDamage);

	int patternId = GetSmartRandomPattern(1, 2);
	if (patternId == 1) PatternCramSession(finalDamage);
	else PatternSubmissionRush(finalDamage);
}

void Demo::DeadlineEnemy::PatternCramSession(float projDamage) {
	constexpr int COLUMNS = 8;
	constexpr float SPAN = 220.f;
	constexpr float STEP = SPAN / (COLUMNS - 1);
	constexpr int WAVES = 10;
	constexpr float SPAWN_Y = -260.f;
	constexpr float BULLET_SPEED = 230.f;
	constexpr float WAVE_DELAY = 0.55f;
	constexpr float HOMING_VELOCITY = 200.f;
	constexpr float HOMING_TURN_SPEED = 1.6f;

	int gap = RNG::Range(0, COLUMNS - 2);
	for (int w = 0; w < WAVES; w++) {
		for (int c = 0; c < COLUMNS; c++) {
			if (c == gap || c == gap + 1) continue;
			float x = -SPAN * 0.5f + c * STEP;
			commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, x, BULLET_SPEED, SPAWN_Y](std::function<void(void)> markFinished) {
				if (auto lock = this->player.lock()) {
					projectiles.Spawn(
						lock,
						ProjectileDesc(projTexture.get(), 8, 8, 12, 12, x, SPAWN_Y)
						.SetTrajectory(D3DXVECTOR2(0.f, 1.f))
						.SetVelocity(BULLET_SPEED)
						.SetDecayTime(4.f)
						.SetDamage(projDamage)
					);
				}
				markFinished();
				}));
			commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.01f));
		}

		if (w % 3 == 2) {
			bool fromRight = RNG::Range(0, 1) == 0;
			float cornerX = fromRight ? 300.f : -300.f;
			commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, cornerX](std::function<void(void)> markFinished) {
				if (auto lock = this->player.lock()) {
					D3DXVECTOR2 dir(cornerX > 0.f ? -1.f : 1.f, 1.f);
					auto desc = ProjectileDesc(projTexture.get(), 8, 8, 12, 12, cornerX, -200.f)
						.SetTrajectory(dir)
						.SetHoming(HOMING_TURN_SPEED)
						.SetVelocity(HOMING_VELOCITY)
						.SetDecayTime(5.f)
						.SetDamage(projDamage);
					desc.SetStatusEffect(
						ModifierType::Freeze, FREEZE_VALUE, FREEZE_DURATION
					);
					projectiles.Spawn(lock, desc);
				}
				markFinished();
				}));
		}

		gap = std::clamp(gap + RNG::Range(-1, 1), 0, COLUMNS - 2);
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(WAVE_DELAY));
	}
}

void Demo::DeadlineEnemy::PatternSubmissionRush(float projDamage) {
	const int DROP_COUNT = 45;
	const float DROP_SPAWN_DELAY = 0.05f;
	const float DROP_SPEED = 200.f;
	const float DROP_HEIGHT = 300.f;
	const float OFFSET_MIN = -300.f;
	const float OFFSET_MAX = 100.f;

	for (int i = 0; i < DROP_COUNT; i++) {
		float offsetX = RNG::Range(OFFSET_MIN, OFFSET_MAX);
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, offsetX, DROP_SPEED, DROP_HEIGHT](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				auto [px, py] = lock->GetWorldPosition();
				projectiles.Spawn(
					lock,
					ProjectileDesc(projTexture.get(), 8, 8, 12, 12, px + offsetX, py - DROP_HEIGHT)
					.SetTrajectory(D3DXVECTOR2(0.4f, 1.f))
					.SetVelocity(DROP_SPEED)
					.SetDecayTime(4.f)
					.SetDamage(projDamage)
				);
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(DROP_SPAWN_DELAY));
	}

	const int SWOOP_COUNT = 4;
	const float SWOOP_SPEED = 380.f;
	const float SWOOP_ACCEL = 200.f;

	for (int i = 0; i < SWOOP_COUNT; i++) {
		bool fromRight = (i % 2 == 0);
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, fromRight, SWOOP_SPEED, SWOOP_ACCEL](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				auto [px, py] = lock->GetWorldPosition();
				float spawnX = fromRight ? 320.f : -320.f;
				float spawnY = RNG::Range(-100.f, 100.f);
				auto desc = ProjectileDesc(projTexture.get(), 8, 8, 12, 12, spawnX, spawnY)
					.SetTargetPosition(px, py)
					.SetInitialVelocity(SWOOP_SPEED)
					.SetReturnAcceleration(SWOOP_ACCEL)
					.SetDecayTime(5.f)
					.SetDamage(projDamage);
				desc.SetStatusEffect(
					ModifierType::Freeze, FREEZE_VALUE, FREEZE_DURATION
				);
				projectiles.Spawn(lock, desc);
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.6f));
	}
}