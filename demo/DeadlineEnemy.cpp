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
	if (patternId == 1) PatternClockTickCountdown(finalDamage);
	else PatternServerMaintenance2359(finalDamage);
}

void Demo::DeadlineEnemy::PatternClockTickCountdown(float projDamage) {
	constexpr int TICKS = 8;
	constexpr int HAND_BULLETS = 7;         
	constexpr float TICK_INTERVAL = 0.55f;
	constexpr float SWEEP_RADIAL_SPEED = 150.f; 
	constexpr float SWEEP_ANGULAR_SPEED = 2.5f;
	constexpr float ANGLE_STEP = 3.14159f * 0.35f;
	constexpr float HAND_DECAY_TIME = 4.5f; 

	constexpr float ARENA_HALF = 128.f;
	constexpr float PIVOT_MARGIN = 50.f; 
	constexpr float PIVOT_A_X = -(ARENA_HALF + PIVOT_MARGIN), PIVOT_A_Y = (ARENA_HALF + PIVOT_MARGIN); 
	constexpr float PIVOT_B_X = (ARENA_HALF + PIVOT_MARGIN), PIVOT_B_Y = -(ARENA_HALF + PIVOT_MARGIN);  

	auto angleA = std::make_shared<float>(0.f);
	auto angleB = std::make_shared<float>(3.14159f * 0.5f); 

	for (int t = 0; t < TICKS; t++) {
		bool useA = (t % 2 == 0);
		float pivotX = useA ? PIVOT_A_X : PIVOT_B_X;
		float pivotY = useA ? PIVOT_A_Y : PIVOT_B_Y;
		float spinDir = useA ? SWEEP_ANGULAR_SPEED : -SWEEP_ANGULAR_SPEED;
		auto angleRef = useA ? angleA : angleB;

		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, angleRef, pivotX, pivotY, spinDir, HAND_BULLETS, SWEEP_RADIAL_SPEED, ANGLE_STEP, HAND_DECAY_TIME](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				float startAng = *angleRef;
				for (int i = 1; i <= HAND_BULLETS; ++i) {
					projectiles.Spawn(
						lock,
						ProjectileDesc(projTexture.get(), 8, 8, 12, 12, pivotX, pivotY)
						.SetSpiralParams(startAng, SWEEP_RADIAL_SPEED * (i / (float)HAND_BULLETS), spinDir)
						.SetVelocity(100.f)
						.SetDecayTime(HAND_DECAY_TIME)
						.SetDamage(projDamage)
					);
				}
				*angleRef += ANGLE_STEP;
			}
			markFinished();
			}));

		if (t % 2 == 1) {
			commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage](std::function<void(void)> markFinished) {
				if (auto lock = this->player.lock()) {
					auto [px, py] = lock->GetWorldPosition();
					auto desc = ProjectileDesc(projTexture.get(), 8, 8, 14, 14, px, -260.f)
						.SetTargetPosition(px, py)
						.SetVelocity(380.f)
						.SetDecayTime(3.0f)
						.SetDamage(projDamage);
					desc.SetStatusEffect(ModifierType::Freeze, FREEZE_VALUE, FREEZE_DURATION);
					projectiles.Spawn(lock, desc);
				}
				markFinished();
				}));
		}

		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(TICK_INTERVAL));
	}
}

void Demo::DeadlineEnemy::PatternServerMaintenance2359(float projDamage) {
	constexpr float BEAM_LENGTH = 300.f;
	constexpr float WARN_TIME = 0.8f;
	constexpr float FIRE_TIME = 4.0f;
	constexpr D3DCOLOR WARN_COLOR = D3DCOLOR_ARGB(160, 255, 60, 60);
	constexpr D3DCOLOR GLOW_COLOR = D3DCOLOR_ARGB(120, 200, 20, 20);
	constexpr D3DCOLOR CORE_COLOR = 0xFFFFFFFF;

	commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, BEAM_LENGTH, WARN_TIME, FIRE_TIME, WARN_COLOR, GLOW_COLOR, CORE_COLOR](std::function<void(void)> markFinished) {
		if (auto lock = this->player.lock()) {
			float wallOffset = 55.f; 
			LaserDesc leftBeam = LaserDesc::Vertical(-wallOffset, 0.f, BEAM_LENGTH)
				.SetWarnTime(WARN_TIME)
				.SetFireTime(FIRE_TIME)
				.SetDamage(projDamage * 1.5f)
				.SetColors(WARN_COLOR, GLOW_COLOR, CORE_COLOR);
			leftBeam.SetStatusEffect(ModifierType::Burn, BURN_VALUE, BURN_DURATION);

			LaserDesc rightBeam = LaserDesc::Vertical(wallOffset, 0.f, BEAM_LENGTH)
				.SetWarnTime(WARN_TIME)
				.SetFireTime(FIRE_TIME)
				.SetDamage(projDamage * 1.5f)
				.SetColors(WARN_COLOR, GLOW_COLOR, CORE_COLOR);
			rightBeam.SetStatusEffect(ModifierType::Burn, BURN_VALUE, BURN_DURATION);

			projectiles.Spawn(lock, leftBeam);
			projectiles.Spawn(lock, rightBeam);
		}
		markFinished();
		}));

	commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(WARN_TIME));

	const int WAVE_SHOTS = 22;
	const float SHOT_DELAY = 0.16f;

	for (int i = 0; i < WAVE_SHOTS; i++) {
		commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([this, projDamage, i](std::function<void(void)> markFinished) {
			if (auto lock = this->player.lock()) {
				float spawnX = RNG::Range(-45.f, 45.f); 
				auto desc = ProjectileDesc(projTexture.get(), 8, 8, 12, 12, spawnX, -220.f)
					.SetTrajectory(D3DXVECTOR2(0.f, 1.f))
					.SetWave(45.f, 5.0f)
					.SetVelocity(280.f)
					.SetDecayTime(3.5f)
					.SetDamage(projDamage);
				projectiles.Spawn(lock, desc);
			}
			markFinished();
			}));
		commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(SHOT_DELAY));
	}

	commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.5f));
}