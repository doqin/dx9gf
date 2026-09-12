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

	groupProjTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	groupProjTexture->LoadTexture(L"assets/placeholder-round-projectile.png"); // TODO: change when real asset is available

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

void Demo::HomeworkEnemy::OnTurnBegin(std::shared_ptr<Player> player, std::shared_ptr<PopUpMessage> popUpMessage, int currentTurn) {
	this->player = player;

	if (abilityCooldown > 0) {
		--abilityCooldown;
		return;
	}

	if (auto lock = this->player.lock()) {
		lock->AddModifier(ModifierType::InvertedControls, CONTROL_INVERT_DURATION, 0.f, false);
	}

	if (popUpMessage) {
		popUpMessage->ShowMessage(L"Homework scrambles your controls!");
	}

	abilityCooldown = 2;
}

void Demo::HomeworkEnemy::StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies, std::shared_ptr<PopUpMessage> popUpMessage, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, int currentTurn) {
	(void)enemies; (void)popUpMessage; (void)graphicsDevice; (void)camera; (void)currentTurn;
	this->player = player;

	const float baseDamage = 2.f;
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
	const float ARENA_HALF_WIDTH = 400.f;
	const float ARENA_HALF_HEIGHT = 300.f;
	const float CLUSTER_VELOCITY = 150.f;
	const float HOLD_TIME = 2.f;
	const int BURST_COUNT = 24;
	const float BURST_VELOCITY = 90.f;
	const float PULSE_COUNT = 10;
	const float PULSE_GAP = 0.5f;
	const float PULSE_DECAY = 0.05f;
	const float RING_DECAY_TIME = 6.f;
	const float RING_BULLET_SIZE = 12.f;

	int edge = RNG::Range(0, 3);
	float spawnX = 0.f, spawnY = 0.f;
	switch (edge) {
	case 0: spawnX = 0.f;               spawnY = -ARENA_HALF_HEIGHT; break;
	case 1: spawnX = 0.f;               spawnY = ARENA_HALF_HEIGHT;  break;
	case 2: spawnX = -ARENA_HALF_WIDTH; spawnY = 0.f;                break;
	default: spawnX = ARENA_HALF_WIDTH; spawnY = 0.f;                break;
	}

	commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, spawnX, spawnY, CLUSTER_VELOCITY, HOLD_TIME, BURST_COUNT, BURST_VELOCITY, RING_DECAY_TIME, RING_BULLET_SIZE](std::function<void(void)> markFinished) {
		if (auto lock = this->player.lock()) {
			auto desc = ProjectileDesc(groupProjTexture.get(), 8, 8, 16, 16, spawnX, spawnY)
				.SetVelocity(CLUSTER_VELOCITY)
				.SetSplitOnArrival(0.f, 0.f, BURST_COUNT, BURST_VELOCITY)
				.SetSplitRandomAngle(true)
				.SetSplitHoldTime(HOLD_TIME)
				.SetSplitDecayTime(RING_DECAY_TIME)
				.SetShardTexture(groupProjTexture.get(), 8, 8, RING_BULLET_SIZE, RING_BULLET_SIZE)
				.SetDamage(projDamage);
			projectiles.Spawn(lock, desc);
		}
		markFinished();
		}));

	const float maxTravelDistance = (std::max)(ARENA_HALF_WIDTH, ARENA_HALF_HEIGHT);
	const float travelTime = maxTravelDistance / CLUSTER_VELOCITY;
	commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(travelTime + HOLD_TIME));

	for (int pulse = 1; pulse < PULSE_COUNT; pulse++) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(PULSE_GAP));
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, BURST_COUNT, BURST_VELOCITY, RING_DECAY_TIME, PULSE_DECAY, RING_BULLET_SIZE](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				auto desc = ProjectileDesc(groupProjTexture.get(), 8, 8, 0, 0, 0.f, 0.f)
					.SetSplitOnDecay(BURST_COUNT, BURST_VELOCITY)
					.SetSplitRandomAngle(true)
					.SetSplitDecayTime(RING_DECAY_TIME)
					.SetDecayTime(PULSE_DECAY)
					.SetShardTexture(groupProjTexture.get(), 8, 8, RING_BULLET_SIZE, RING_BULLET_SIZE)
					.SetDamage(projDamage);
				projectiles.Spawn(lock, desc);
			}
			markFinished();
			}));
	}

	commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(1.0f));
}