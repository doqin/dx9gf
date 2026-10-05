#include "pch.h"
#include "IEnemy.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include <cmath>
#include <algorithm>
#include "DrawUtils.h"
#include "RNG.h"
#include "DX9GFInputManager.h"
#include "PopUpMessage.h"
#include "StatusDisplay.h"
void Demo::IEnemy::InitCardSpawnTrigger(DX9GF::Camera* camera, float width, float height)
{
	cardSpawnTrigger = std::make_shared<DX9GF::RectangleTrigger>(transformManager, shared_from_this(), width, height);
	cardSpawnTrigger->SetOriginCenter();
	cardSpawnTrigger->Init(camera);
	cardSpawnTrigger->SetOnClickLeft([this](DX9GF::ITrigger* trigger) {
		onRequestEnemyCard(std::dynamic_pointer_cast<IEnemy>(shared_from_this()));
		});
}

void Demo::IEnemy::SetOnRequestEnemyCard(std::function<void(std::shared_ptr<IEnemy>)> callback)
{
	onRequestEnemyCard = callback;
}

void Demo::IEnemy::Update(unsigned long long deltaTime)
{
	if (!isOnStandby) {
		if (cardSpawnTrigger) {
			cardSpawnTrigger->Update(deltaTime);
		}
	}

	timeSinceStart += deltaTime;

	for (auto& indicator : damageIndicators) {
		indicator.elapsed += deltaTime;
		indicator.vy += 800.f * deltaTime / 1000.f; // gravity
		indicator.offsetX += indicator.vx * deltaTime / 1000.f;
		indicator.offsetY += indicator.vy * deltaTime / 1000.f;
	}
	damageIndicators.erase(std::remove_if(damageIndicators.begin(), damageIndicators.end(), [](const DamageIndicator& indicator) {
		return indicator.elapsed >= 700;
		}), damageIndicators.end());
	projectiles.Update(deltaTime);
	commandBuffer.Update(deltaTime);
	animationBuffer.Update(deltaTime);
	UpdateDeath(deltaTime);
}

void Demo::IEnemy::BeginDeath()
{
	if (isDying) return;
	isDying = true;
	deathTimer = 0.f;
	deathBurstSpawned = false;
	if (auto* body = GetBodySprite()) {
		deathBaseScale = body->GetScale().x;
	}
	if (graphicsDevice) {
		deathParticleTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		deathParticleTexture->CreatePlainTexture(0xFFFFFFFF, 4, 4);
		deathParticles = std::make_unique<DX9GF::ParticleSystem>(deathParticleTexture.get(), 60);
		deathParticles->SetOrigin(2.f, 2.f);
		DX9GF::ConfigureExplosionEmitter(*deathParticles);
	}
	DX9GF::AudioManager::GetInstance()->PlayRandom("take_dmg", 1.0f);
}

void Demo::IEnemy::UpdateDeath(unsigned long long deltaTime)
{
	if (!isDying) return;
	deathTimer += static_cast<float>(deltaTime);

	if (!deathBurstSpawned && deathTimer >= DEATH_FLASH_MS) {
		deathBurstSpawned = true;
		if (deathParticles) {
			constexpr float TWO_PI = 6.2831853f;
			for (int i = 0; i < 36; ++i) {
				float ang = RNG::Range(0.f, TWO_PI);
				float speed = RNG::Range(80.f, 300.f);
				deathParticles->Spawn(GetWorldX(), GetWorldY(), ang,
					RNG::Range(0.8f, 1.6f), RNG::Range(0.8f, 1.6f), 0xFFFFFFFF,
					std::cos(ang) * speed, std::sin(ang) * speed);
			}
		}
	}
	if (deathParticles) {
		deathParticles->Update(deltaTime, GetWorldX(), GetWorldY(), 0.f, 1.f, 1.f, 0xFFFFFFFF, false);
	}

	if (auto* body = GetBodySprite()) {
		if (deathTimer < DEATH_FLASH_MS) {
			// Sprite colour only modulates (can't brighten), so the flash is a hard red tint.
			body->SetColor(D3DCOLOR_ARGB(255, 255, 70, 70));
		}
		else {
			const float fadeMs = 450.f;
			float t = std::clamp((deathTimer - DEATH_FLASH_MS) / fadeMs, 0.f, 1.f);
			BYTE a = static_cast<BYTE>(255.f * (1.f - t));
			body->SetColor(D3DCOLOR_ARGB(a, 255, 70, 70));
			body->SetScale(deathBaseScale * (1.f - 0.4f * t));
		}
	}
}

void Demo::IEnemy::Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime)
{
	if (!graphicsDevice || !camera) {
		return;
	}
	this->graphicsDevice = graphicsDevice;

	if (!font) {
		font = std::make_shared<DX9GF::Font>(graphicsDevice, Demo::kMainFontName, Demo::kMainFontSize);
		fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	}
	if (!uiTexture) {
		uiTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		uiTexture->LoadTexture(L"assets/ui.png");
		uiSprite = std::make_shared<DX9GF::StaticSprite>(uiTexture.get());
		uiSprite->SetScale(2.0f);
	}
	if (!statusRenderer) {
		statusRenderer = StatusRenderer::Get(graphicsDevice);
	}

	if (!isOnStandby && cardSpawnTrigger && !isDying) {
		bool isHovered = cardSpawnTrigger->IsHovering(deltaTime);

		const float triggerLeft = cardSpawnTrigger->GetWorldX() - cardSpawnTrigger->GetOriginX();
		const float triggerTop = cardSpawnTrigger->GetWorldY() - cardSpawnTrigger->GetOriginY();
		const float triggerW = cardSpawnTrigger->GetWidth();
		const float triggerH = cardSpawnTrigger->GetHeight();

		// Blink logic (idle state, not hovered)
		float blinkFreq = 1.0f; // blinks per second
		float alphaMult = (sin(timeSinceStart * 0.001f * blinkFreq * 3.14159f) + 1.0f) * 0.5f; // 0 to 1
		int idleAlpha = (int)(64 * alphaMult); // 64 is ~25% of 255

		graphicsDevice->SetAlphaBlending(true);

		if (isHovered) {
			graphicsDevice->DrawRectangle(
				*camera,
				triggerLeft, triggerTop,
				triggerW, triggerH,
				0, 1, 1, 0, 0, D3DCOLOR_ARGB(80, 255, 240, 120), true);

			Demo::DrawAnimatedDashedRectangle(
				graphicsDevice,
				*camera,
				triggerLeft,
				triggerTop,
				triggerW,
				triggerH,
				3.f,
				0xFFFFE678,
				false,
				4.f,
				0xFFFFE678,
				20.f,
				10.f,
				40.f,
				GetTickCount64()
			);

			{
				const float centerX = triggerLeft + triggerW / 2.f;
				const float centerY = triggerTop + triggerH / 2.f;
				const float plusLength = 40.f;
				const float plusThickness = 10.f;

				const float outlinePad = 3.f;
				graphicsDevice->DrawRectangle(
					*camera,
					centerX - plusLength / 2.f - outlinePad, centerY - plusThickness / 2.f - outlinePad,
					plusLength + outlinePad * 2.f, plusThickness + outlinePad * 2.f,
					0xFF000000, true);
				graphicsDevice->DrawRectangle(
					*camera,
					centerX - plusThickness / 2.f - outlinePad, centerY - plusLength / 2.f - outlinePad,
					plusThickness + outlinePad * 2.f, plusLength + outlinePad * 2.f,
					0xFF000000, true);

				graphicsDevice->DrawRectangle(
					*camera,
					centerX - plusLength / 2.f, centerY - plusThickness / 2.f,
					plusLength, plusThickness,
					0xFFFFE678, true);
				graphicsDevice->DrawRectangle(
					*camera,
					centerX - plusThickness / 2.f, centerY - plusLength / 2.f,
					plusThickness, plusLength,
					0xFFFFE678, true);
			}
		}
		else {
			D3DCOLOR color = D3DCOLOR_ARGB(idleAlpha, 128, 128, 128);
			graphicsDevice->DrawRectangle(
				*camera,
				triggerLeft, triggerTop,
				triggerW, triggerH,
				0, 1, 1, 0, 0, color, true);

			if (fontSprite) {
				fontSprite->Begin();
				fontSprite->SetOutline(true, 0xFF000000, 2.f);
				fontSprite->SetColor(D3DCOLOR_ARGB((std::max)(idleAlpha, 140), 255, 230, 120));
				std::wstring hintText = Tr(L"Click to get enemy card");
				fontSprite->SetText(std::move(hintText));
				float hintWidth = fontSprite->GetWidth();
				fontSprite->SetPosition(GetWorldX() - hintWidth / 2.f, triggerTop - 24.f);
				fontSprite->Draw(*camera, deltaTime);
				fontSprite->SetOutline(false);
				fontSprite->End();
			}
		}

		graphicsDevice->SetAlphaBlending(false);
	}

	if (cardSpawnTrigger && !isDying) {
		cardSpawnTrigger->Draw(graphicsDevice, *camera);
	}

	const auto currentHealth = static_cast<int>(std::round(health));
	const auto totalHealth = static_cast<int>(std::round(maxHealth));
	auto healthText = std::to_wstring(health) + L"/" + std::to_wstring(maxHealth);

	fontSprite->Begin();
	fontSprite->SetOutline(true, 0xFF000000, 2.f);
	fontSprite->SetColor(0xFFFFFFFF);
	fontSprite->SetPosition(GetWorldX(), GetWorldY() - 105.f);
	fontSprite->SetText(std::move(healthText));
	if (!isDying) {
		fontSprite->Draw(*camera, deltaTime);
	}

	for (auto it = hitImpactSprites.begin(); it != hitImpactSprites.end(); ) {
		auto& sprite = *it;
		sprite->Begin();
		sprite->Draw(*camera, deltaTime);
		sprite->End();
		if (sprite->IsFinished()) {
			it = hitImpactSprites.erase(it);
		}
		else {
			++it;
		}
	}

	for (size_t i = 0; i < damageIndicators.size(); ++i) {
		auto text = damageIndicators[i].text;
		fontSprite->SetColor(damageIndicators[i].textColor);
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		fontSprite->SetBold(true);
		fontSprite->SetScale(2.f);
		fontSprite->SetPosition(GetWorldX() + damageIndicators[i].offsetX, GetWorldY() - 30.f + damageIndicators[i].offsetY);
		fontSprite->SetText(std::move(text));
		fontSprite->Draw(*camera, deltaTime);
	}
	fontSprite->SetBold(false);
	fontSprite->SetOutline(false);
	fontSprite->SetScale(1.f);

	float statusOffsetX = 64.f;
	float statusOffsetY = -20.f;

	auto [screenX, screenY] = DX9GF::InputManager::GetInstance()->GetVirtualAbsoluteMousePos(camera);
	auto [mouseX, mouseY] = DX9GF::Utils::WindowToWorldCoords(*camera, screenX, screenY);

	for (const auto& mod : modifiers) {
		if (isDying) break;
		if (mod.type == ModifierType::BuffDefense && mod.value <= 0.f) continue;
		if (mod.duration <= 0) continue;

		auto status = DescribeStatus(mod);
		if (!status || !statusRenderer) continue;

		float iconX = GetWorldX() + statusOffsetX;
		float iconY = GetWorldY() + statusOffsetY;

		// The renderer draws with its own sprites, so the fontSprite batch stays open around it.
		const float rowWidth = statusRenderer->DrawRow(*camera, deltaTime, iconX, iconY, *status);

		bool isHovered = mouseX >= iconX && mouseX <= iconX + rowWidth
			&& mouseY >= iconY && mouseY <= iconY + StatusRenderer::ROW_HEIGHT;
		if (isHovered) {
			fontSprite->SetText(status->Tooltip());
			float tooltipWidth = fontSprite->GetWidth() + 8.f;
			float tooltipHeight = fontSprite->GetHeight() + 8.f;

			auto [screenW, screenH] = camera->GetScreenResolution();
			float targetScreenX = screenX;
			float targetScreenY = screenY - tooltipHeight;

			if (targetScreenX + tooltipWidth > screenW) {
				targetScreenX = screenW - tooltipWidth;
			}
			if (targetScreenY < 0) {
				targetScreenY = screenY + 32.f;
			}

			auto [worldDrawX, worldDrawY] = DX9GF::Utils::WindowToWorldCoords(*camera, targetScreenX, targetScreenY);

			fontSprite->End();
			graphicsDevice->SetAlphaBlending(true);
			graphicsDevice->DrawRectangle(*camera, worldDrawX, worldDrawY, tooltipWidth, tooltipHeight, 0, 1, 1, 0, 0, D3DCOLOR_ARGB(220, 0, 0, 0), true);
			fontSprite->Begin();

			fontSprite->SetColor(0xFFFFFFFF);
			fontSprite->SetOutline(true, 0xFF000000, 1.f);
			fontSprite->SetPosition(worldDrawX + 4.f, worldDrawY + 4.f);
			fontSprite->Draw(*camera, deltaTime);
		}

		statusOffsetY += 36.f;
	}

	fontSprite->End();
	projectiles.Draw(graphicsDevice, *camera, deltaTime);

	if (deathParticles) {
		graphicsDevice->SetAlphaBlending(true);
		deathParticles->Draw(*camera, deltaTime);
		graphicsDevice->SetAlphaBlending(false);
	}
}

bool Demo::IEnemy::TakeDamage(float damage, bool ignoreArmor)
{
	// Vulnerable/Weak can leave fractions; health is shown rounded, so a leftover 0.25 would read as 0 HP but alive.
	const int actualDamage = static_cast<int>(std::round(CalculateActualDamage(damage, ignoreArmor)));
	health -= actualDamage;
	if (health < 0) health = 0;

	if (actualDamage > 0) DX9GF::AudioManager::GetInstance()->PlayRandom("take_dmg", 0.8f);

	damageIndicators.push_back(DamageIndicator{
		L"-" + std::to_wstring(static_cast<int>(std::round(actualDamage))),
		0.f,
		0.f,
		RNG::Range(-64.f, 64.f),
		RNG::Range(-200.f, -100.f),
		0,
		0xFFFF4444
		});

	if (!hitImpactTexture && graphicsDevice) {
		hitImpactTexture = std::make_shared<DX9GF::Texture>(graphicsDevice);
		hitImpactTexture->LoadTexture(L"assets/hitimpact-Sheet.png");
	}
	hitImpactSprites.push_back(std::make_shared<DX9GF::AnimatedSprite>(hitImpactTexture.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 32, 32, 4), 24, false));

	hitImpactSprites.back()->SetPosition(GetWorldX() + RNG::Range(-16.f, 16.f), GetWorldY() + RNG::Range(-16.f, 16.f));
	hitImpactSprites.back()->SetScale(RNG::Range(2.f, 3.f));
	hitImpactSprites.back()->SetRotation(RNG::Range(-0.5f, 0.5f));

	float ox = GetWorldX();
	float oy = GetWorldY();

	if (!animationBuffer.IsBusy()) {
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox - 8.f, oy - 6.f, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox + 8.f, oy + 6.f, 0.1f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox - 4.f, oy - 4.f, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox + 4.f, oy + 2.f, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox, oy, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
	}

	if (IsDead()) BeginDeath();
	return IsDead();
}

bool Demo::IEnemy::TakeIndirectDamage(float damage, DamageType type) {
	const int dealt = static_cast<int>(std::round(damage));
	health -= dealt;
	if (health < 0) health = 0;

	D3DCOLOR textColor = 0xFFFFFFFF;

	if (type == DamageType::Poison) {
		textColor = 0xFFba4aed; //purple
		DX9GF::AudioManager::GetInstance()->PlayRandom("take_dmg", 0.5f);
	}
	else if (type == DamageType::Burn) {
		textColor = 0xFFff8800; //orange
		DX9GF::AudioManager::GetInstance()->PlayRandom("take_dmg", 0.5f);
	}

	float ox = GetWorldX();
	float oy = GetWorldY();
	if (!animationBuffer.IsBusy()) {
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox - 2.f, oy, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox + 2.f, oy, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
		animationBuffer.PushCommand(std::make_shared<DX9GF::GoToCommand>(shared_from_this(), ox, oy, 0.05f, DX9GF::TimeTag{}, DX9GF::EaseInOutTag{}));
	}

	damageIndicators.push_back(DamageIndicator{
		L"-" + std::to_wstring(dealt),
		0.f,
		0.f,
		RNG::Range(-32.f, 32.f),
		RNG::Range(-150.f, -100.f),
		0,
		textColor
		});

	if (IsDead()) BeginDeath();
	return IsDead();
}

void Demo::IEnemy::SetState(bool isOnStandby)
{
	this->isOnStandby = isOnStandby;
}

bool Demo::IEnemy::IsDoneAttacking()
{
	const bool deathDone = !isDying || deathTimer >= DEATH_DURATION_MS;
	return !commandBuffer.IsBusy() && !animationBuffer.IsBusy() && projectiles.IsEmpty() && hitImpactSprites.empty() && deathDone;
}

int Demo::IEnemy::GetSmartRandomPattern(const int&& minPattern, const int&& maxPattern)
{
	if (minPattern > maxPattern) {
		throw std::invalid_argument("minPattern cannot be greater than maxPattern");
	}
	
	std::vector<int>& patternOrder = GetPatternOrder();
	size_t& currentIndex = GetCurrentPatternIndex();

	// Initialize the pattern order vector with values from minPattern to maxPattern
	if (patternOrder.size() != static_cast<size_t>(maxPattern - minPattern + 1)) {
		patternOrder.resize(maxPattern - minPattern + 1);
		currentIndex = 0;
	}

	if (currentIndex == 0) {
		for (int i = minPattern; i <= maxPattern; ++i) {
			patternOrder[i - minPattern] = i;
		}
		std::shuffle(patternOrder.begin(), patternOrder.end(), RNG::GetEngine());
	}
	
	int patternId = patternOrder[currentIndex];
	currentIndex = (currentIndex + 1) % patternOrder.size();
	return patternId;
}

void Demo::IEnemy::SpawnHealText(int actualHeal) {
	damageIndicators.push_back(DamageIndicator{
		L"+" + std::to_wstring(static_cast<int>(std::round(actualHeal))),
		0.f,
		0.f,
		RNG::Range(-32.f, 32.f),
		RNG::Range(-150.f, -100.f),
		0,
		0xFF59c135 //green
		});
}

void Demo::IEnemy::CastAbility(std::function<void()> effect, std::shared_ptr<PopUpMessage> popUpMessage, const std::wstring& message) {
	commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([effect](std::function<void(void)> markFinished) {
		if (effect) effect();
		markFinished();
		}));


	commandBuffer.PushCommand(std::make_shared<DX9GF::CustomCommand>([popUpMessage, message](std::function<void(void)> markFinished) {
		if (popUpMessage && !message.empty()) {
			popUpMessage->ShowMessage(message, 3.f);
		}
		markFinished();
	}));
	commandBuffer.PushCommand(std::make_shared<DX9GF::DelayCommand>(0.5f));
}