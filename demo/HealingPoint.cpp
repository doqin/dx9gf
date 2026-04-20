#include "pch.h"
#include "HealingPoint.h"
#include <cmath>

namespace Demo {
    HealingPoint::HealingPoint(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
        : IGameObject(tm, x, y), transformManager(tm) {
    }

    void HealingPoint::Init(DX9GF::GraphicsDevice* gd, DX9GF::Camera* camera, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font) {
        player = p;
        fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
        this->gd = gd;
        collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, 32.f, 32.f, GetWorldX(), GetWorldY());
        collider->SetOriginCenter();
        cm->Add(collider);

        spritesheet = std::make_shared<DX9GF::Texture>(gd);
        spritesheet->LoadTexture(L"savepoint-Sheet.png");

        sprite = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 32, 32, 10), 12, true);
        sprite->SetOrigin(16.f, 16.f);
        sprite->SetPosition(GetWorldX(), GetWorldY());
    }

    void HealingPoint::Update(unsigned long long deltaTime) {
        if (!isVisible) return;

        if (messageTimer > 0) {
            messageTimer -= deltaTime / 1000.0f;
            if (messageTimer <= 0) statusMessage = "";
        }

        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = GetWorldPosition();

        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        isPlayerNear = (distance <= INTERACTION_DISTANCE);

        auto inpMan = DX9GF::InputManager::GetInstance();

        if (isPlayerNear && inpMan->KeyPress(DIK_E)) {
            if (pLock->GetHealth() < pLock->GetMaxHealth()) {
                pLock->SetHealth(pLock->GetMaxHealth());
                statusMessage = "Healed to FULL HP!";
                messageTimer = 1.5f;
            }
            else {
                statusMessage = "HP is already full!";
                messageTimer = 1.0f;
            }
        }
    }

    void HealingPoint::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        if (!isVisible) return;

        auto [x, y] = GetWorldPosition();

        sprite->Begin();
        sprite->Draw(camera, deltaTime);
        sprite->End();

        if (fontSprite) {
            fontSprite->Begin();

            if (!statusMessage.empty()) {
                fontSprite->SetText(std::wstring(statusMessage.begin(), statusMessage.end()));
                fontSprite->SetScale(0.8f);
                fontSprite->SetColor(0xFF55FF55);
                fontSprite->SetPosition(x - fontSprite->GetWidth() / 2.f, y - 45.f);
                fontSprite->SetOutline(true, 0xFF000000);
                fontSprite->Draw(camera, deltaTime);
            }
            else if (isPlayerNear) {
                fontSprite->SetText(L"E");
                fontSprite->SetScale(1.f);
                fontSprite->SetColor(0xFFFFFFFF);
                fontSprite->SetPosition(x - fontSprite->GetWidth() / 2.f, y - 30.f - fontSprite->GetHeight() / 2.f);
                fontSprite->SetOutline(true, 0xFF000000);
                fontSprite->Draw(camera, deltaTime);
            }
            fontSprite->End();
        }
    }
}