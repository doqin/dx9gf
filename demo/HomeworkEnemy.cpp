#include "pch.h"
#include "HomeworkEnemy.h"
#include "resource.h"
#include "RNG.h"
#include "PopUpMessage.h"

void Demo::HomeworkEnemy::Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera) {
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

void Demo::HomeworkEnemy::Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime) {
	if (sprite) {
		sprite->Begin();
		auto [x, y] = GetWorldPosition();
		sprite->SetPosition(x, y);
		sprite->Draw(*camera, deltaTime);
		sprite->End();
	}
	IEnemy::Draw(graphicsDevice, camera, deltaTime);
}

void Demo::HomeworkEnemy::StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies, std::shared_ptr<PopUpMessage> popUpMessage, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, int currentTurn) {
	(void)enemies; (void)popUpMessage; (void)graphicsDevice; (void)camera; (void)currentTurn;
	this->player = player;

	const float baseDamage = 1.f;
	const float finalDamage = CalculateOutgoingDamage(baseDamage);

	int patternId = GetSmartRandomPattern(1, 2);
	if (patternId == 1) PatternPileUp(finalDamage);
	else PatternGroupProject(finalDamage);
}

void Demo::HomeworkEnemy::PatternPileUp(float projDamage) {
	const int JUNK_COUNT = 40;
	const float JUNK_DELAY = 0.08f;
	const float JUNK_INITIAL_VELOCITY = 350.f; 
	const float JUNK_RETURN_ACCEL = 120.f;   

	for (int i = 0; i < JUNK_COUNT; i++) {
		float randY = RNG::Range(-160.f, 160.f);
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, randY, JUNK_INITIAL_VELOCITY, JUNK_RETURN_ACCEL](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				auto [px, py] = lock->GetWorldPosition();

				auto desc = ProjectileDesc(projTexture.get(), 8, 8, 12, 12, 320.f, 0.f)
					.SetTargetPosition(px, py + randY)
					.SetInitialVelocity(JUNK_INITIAL_VELOCITY)
					.SetReturnAcceleration(JUNK_RETURN_ACCEL)
					.SetDecayTime(6.f)
					.SetDamage(projDamage);
				desc.SetStatusEffect(
					ModifierType::Burn, BURN_VALUE, BURN_DURATION
				);
				projectiles.Spawn(lock, desc);
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(JUNK_DELAY));
	}

	const int SINE_BULLETS = 6;
	const int SINE_SPACING = 110;
	const float SINE_VELOCITY = 100.f;
	const float SINE_AMPLITUDE = 40.f;
	const float SINE_SPAWN_X = 340.f;

	auto sineWave = std::make_shared<DX9GF::CustomCommand>([this, projDamage, SINE_BULLETS, SINE_SPACING, SINE_VELOCITY, SINE_AMPLITUDE, SINE_SPAWN_X](std::function<void(void)> markFinished) {
		if (auto lock = this->player.lock()) {
			bool fromLeft = RNG::Range(0, 1) == 0;
			float spawnX = fromLeft ? -SINE_SPAWN_X : SINE_SPAWN_X;
			D3DXVECTOR2 dir(fromLeft ? 1.f : -1.f, 0.f);
			for (int i = 0; i < SINE_BULLETS; i++) {
				float startY = (i - SINE_BULLETS / 2.f) * SINE_SPACING;
				auto desc = ProjectileDesc(projTexture.get(), 8, 8, 12, 12, spawnX, startY)
					.SetTrajectory(dir)
					.SetWave(SINE_AMPLITUDE, 3.5f)
					.SetDelay(i * 0.08f)
					.SetVelocity(SINE_VELOCITY)
					.SetDecayTime(6.f)
					.SetDamage(projDamage);
				desc.SetStatusEffect(
					ModifierType::Burn, BURN_VALUE, BURN_DURATION
				);
				projectiles.Spawn(lock, desc);
			}
		}
		markFinished();
		});

	for (int i = 0; i < 6; i++) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>(*sineWave));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.7f));
	}
}

void Demo::HomeworkEnemy::PatternGroupProject(float projDamage) {
	const int WAVE_COUNT = 6;
	const int BULLET_PER_WAVE = 8;
	const float WAVE_DELAY = 1.0f;
	const float BULLET_SPEED = 160.f;
	const float DROP_HEIGHT = 300.f;
	const float BULLET_SPACING = 80.f;
	const float WALL_START_X = -(BULLET_PER_WAVE / 2.f) * BULLET_SPACING;

	for (int wave = 0; wave < WAVE_COUNT; wave++) {
		int emptyHole = RNG::Range(0, BULLET_PER_WAVE - 1);
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, emptyHole, BULLET_PER_WAVE, BULLET_SPEED, DROP_HEIGHT, WALL_START_X, BULLET_SPACING](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				for (int i = 0; i < BULLET_PER_WAVE; i++) {
					if (i == emptyHole) continue;
					float x = WALL_START_X + i * BULLET_SPACING;
					projectiles.Spawn(
						lock,
						ProjectileDesc(projTexture.get(), 8, 8, 12, 12, x, -DROP_HEIGHT)
						.SetTrajectory(D3DXVECTOR2(0.f, 1.f))
						.SetVelocity(BULLET_SPEED)
						.SetDecayTime(6.f)
						.SetDamage(projDamage)
					);
				}
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(WAVE_DELAY));
	}

	const int ZIGZAG_WAVES = 5;
	const int ZIGZAG_BULLETS = 8;
	const int STEP_Y = 100;
	const float START_Y = -(ZIGZAG_BULLETS - 1) * STEP_Y * 0.5f;

	for (int wave = 0; wave < ZIGZAG_WAVES; wave++) {
		bool fromLeft = (wave % 2 == 0);
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, fromLeft, START_Y, STEP_Y, ZIGZAG_BULLETS](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				float spawnX = fromLeft ? -300.f : 300.f;
				D3DXVECTOR2 dir(fromLeft ? 1.f : -1.f, 0.f);
				for (int i = 0; i < ZIGZAG_BULLETS; i++) {
					float spawnY = START_Y + STEP_Y * static_cast<float>(i);
					projectiles.Spawn(
						lock,
						ProjectileDesc(projTexture.get(), 8, 8, 12, 12, spawnX, spawnY)
						.SetTrajectory(dir)
						.SetVelocity(100.f)
						.SetWave(80.f, 0.3f, true)
						.SetDecayTime(7.f)
						.SetDamage(projDamage)
					);
				}
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.5f));
	}
}