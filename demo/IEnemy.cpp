#include "pch.h"
#include "IEnemy.h"
#include <cmath>
#include <algorithm>

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

bool Demo::IEnemy::IsDead() const
{
    return health < 1;
}

void Demo::IEnemy::Update(unsigned long long deltaTime)
{
  if (cardSpawnTrigger) {
        cardSpawnTrigger->Update(deltaTime);
    }
  for (auto& indicator : damageIndicators) {
        indicator.elapsed += deltaTime;
        indicator.offsetY -= 20.f * deltaTime / 1000.f;
    }
    damageIndicators.erase(std::remove_if(damageIndicators.begin(), damageIndicators.end(), [](const DamageIndicator& indicator) {
        return indicator.elapsed >= 700;
    }), damageIndicators.end());
}

void Demo::IEnemy::Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime)
{
    if (!graphicsDevice || !camera) {
        return;
    }
    if (!damageFont) {
        damageFont = std::make_shared<DX9GF::Font>(graphicsDevice, L"Arial", 18);
        damageFontSprite = std::make_shared<DX9GF::FontSprite>(damageFont.get());
    }
    if (cardSpawnTrigger) {
        cardSpawnTrigger->Draw(graphicsDevice, camera);
    }

    const auto currentHealth = static_cast<int>(std::round(health));
    const auto totalHealth = static_cast<int>(std::round(maxHealth));
    auto healthText = std::to_wstring(currentHealth) + L"/" + std::to_wstring(totalHealth);

    damageFontSprite->Begin();
    damageFontSprite->SetColor(0xFFFFFFFF);
    damageFontSprite->SetPosition(GetWorldX(), GetWorldY() - 48.f);
    damageFontSprite->SetText(std::move(healthText));
    damageFontSprite->Draw(*camera, deltaTime);

   for (size_t i = 0; i < damageIndicators.size(); ++i) {
        auto text = damageIndicators[i].text;
        damageFontSprite->SetColor(0xFFFF4444);
     damageFontSprite->SetPosition(GetWorldX(), GetWorldY() - 30.f + damageIndicators[i].offsetY - i * 14.f);
        damageFontSprite->SetText(std::move(text));
        damageFontSprite->Draw(*camera, deltaTime);
    }
    damageFontSprite->End();
}

bool Demo::IEnemy::TakeDamage(float damage)
{
    health -= damage;
    if (health < 0) health = 0;
  damageIndicators.push_back(DamageIndicator{
        L"-" + std::to_wstring(static_cast<int>(std::round(damage))),
        0.f,
        0
    });
    return IsDead();
}
