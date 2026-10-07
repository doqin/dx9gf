#include "pch.h"
#include "SettingsManager.h"
#include "SavePoint.h"
#include "PopupManager.h"
#include "PlayerGlobalData.h"
#include "LocalizationManager.h"
#include <cmath>

namespace Demo {
    SavePoint::SavePoint(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
        : IGameObject(tm, x, y), transformManager(tm) {
    }

    void SavePoint::Init(DX9GF::GraphicsDevice* gd, DX9GF::Camera* worldCamera, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::SaveManager> sm, std::shared_ptr<DX9GF::Font> font, std::shared_ptr<DX9GF::CommandBuffer> drawBuffer) {
        this->player = p;
        this->worldCamera = worldCamera;
        this->saveManager = sm;
        this->fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
        this->drawBuffer = drawBuffer;
        this->gd = gd;

        collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, 32.f, 8.f, GetWorldX(), GetWorldY() + 12.f);
        collider->SetOriginCenter();
        cm->Add(collider);

        spritesheet = std::make_shared<DX9GF::Texture>(gd);
        spritesheet->LoadTexture(L"assets/savepoint-Sheet.png");
        sprite = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 32, 32, 10), 12, true);
        sprite->SetOrigin(16.f, 16.f);
        sprite->SetPosition(GetWorldX(), GetWorldY());
    }

    void SavePoint::Update(unsigned long long deltaTime) {
        if (!isVisible) return;

        if (PopupManager::GetInstance()->IsActive()) return;

        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = GetWorldPosition();

        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        isPlayerNear = (distance <= INTERACTION_DISTANCE);

        auto inpMan = DX9GF::InputManager::GetInstance();
        if (isPlayerNear && inpMan->KeyPress(SettingsManager::GetInstance()->GetKeybind("INTERACT"))) {
            // Interacting unlocks the point as a fast travel destination. The unlock is part of
            // the save, so it only survives a reload once the player saves.
            const bool newlyUnlocked = !id.empty() && PlayerGlobalData::GetInstance()->UnlockSavePoint(id);
            if (newlyUnlocked) {
                DX9GF::AudioManager::GetInstance()->Play("checkpoint", false, 0.7f);
            }

            std::vector<std::pair<std::wstring, std::function<void()>>> buttons = {
                { Tr(L"Save"), [this]() {
                    if (auto smLock = this->saveManager.lock()) {
                        smLock->Save("savegame.json");
                        OutputDebugStringA("Successfully saved!\n");
                        DX9GF::AudioManager::GetInstance()->Play("checkpoint", false, 0.7f);
                    }
                }},
                { Tr(L"Open Map"), [this]() {
                    if (onOpenMap) onOpenMap();
                }},
                { Tr(L"Cancel"), nullptr }
            };

            const std::wstring message = newlyUnlocked
                ? Tr(L"Fast travel unlocked! Save the game or open the map?")
                : Tr(L"Save the game or open the map?");
            PopupManager::GetInstance()->Show("basic_blackwhite", Tr(L"SAVE POINT"), message, buttons);
        }
    }

    bool SavePoint::IsUnlocked() const {
        return !id.empty() && PlayerGlobalData::GetInstance()->IsSavePointUnlocked(id);
    }

    void SavePoint::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        if (!isVisible) return;
        indicatorTime += static_cast<float>(deltaTime);
        // A save point that hasn't been used yet looks dormant
        sprite->SetColor(IsUnlocked() ? 0xFFFFFFFF : 0xFF6E7380);
        sprite->Begin();
        sprite->Draw(camera, deltaTime);
        sprite->End();
		DrawPosition(deltaTime, gd, camera);
    }

    void SavePoint::DrawUI(DX9GF::Camera* uiCamera, unsigned long long deltaTime) {
        if (!isVisible || !uiCamera || !worldCamera) return;

        auto [worldX, worldY] = GetWorldPosition();
        float zoom = worldCamera->GetZoom();
        float uiX = (worldX - worldCamera->GetPosition().x) * zoom;
        float uiY = (worldY - worldCamera->GetPosition().y) * zoom;

        // The UI camera can draw into the letterbox bars, so skip anything that isn't fully on screen
        const float cx = std::round(uiX);
        const float cy = std::round(uiY - 24.f * zoom);
        const float u = zoom;  // one world pixel on screen
        const float reach = 8.f * u;
        if (screenW > 0.f && (cx - reach < -screenW / 2.f || cx + reach > screenW / 2.f
            || cy - reach < -screenH / 2.f || cy + reach > screenH / 2.f)) return;

        // Fast travel indicator: a padlock while locked, a pulsing green gem once unlocked
        {
            gd->SetAlphaBlending(true);
            if (IsUnlocked()) {
                const float pulse = 0.5f + 0.5f * std::sin(indicatorTime / 300.f);
                const BYTE glow = static_cast<BYTE>(60 + 80 * pulse);
                gd->DrawRectangle(*uiCamera, cx - 6 * u, cy - 6 * u, 12 * u, 12 * u, D3DCOLOR_ARGB(glow, 102, 224, 122), true);
                gd->DrawRectangle(*uiCamera, cx - 4 * u, cy - 4 * u, 8 * u, 8 * u, 0xFF000000, true);
                gd->DrawRectangle(*uiCamera, cx - 3 * u, cy - 3 * u, 6 * u, 6 * u, 0xFF66E07A, true);
            }
            else {
                // shackle, then body
                gd->DrawRectangle(*uiCamera, cx - 4 * u, cy - 7 * u, 8 * u, 7 * u, 0xFF000000, true);
                gd->DrawRectangle(*uiCamera, cx - 3 * u, cy - 6 * u, 6 * u, 6 * u, 0xFF8A8F99, true);
                gd->DrawRectangle(*uiCamera, cx - 2 * u, cy - 5 * u, 4 * u, 5 * u, 0xFF000000, true);
                gd->DrawRectangle(*uiCamera, cx - 5 * u, cy - 2 * u, 10 * u, 8 * u, 0xFF000000, true);
                gd->DrawRectangle(*uiCamera, cx - 4 * u, cy - 1 * u, 8 * u, 6 * u, 0xFF8A8F99, true);
            }
            gd->SetAlphaBlending(false);
        }

        if (isPlayerNear && !PopupManager::GetInstance()->IsActive()) {

            float scale = 1.0f * zoom;
            fontSprite->Begin();
            fontSprite->SetText(SettingsManager::GetInstance()->GetKeybindDisplayName("INTERACT"));
            float textW = fontSprite->GetWidth() * scale;
            float textH = fontSprite->GetHeight() * scale;

            fontSprite->SetScale(scale);
            fontSprite->SetColor(0xFFFFFFFF);
            fontSprite->SetPosition(uiX - textW / 2.f, uiY - 44.f * zoom - textH / 2.f);
            fontSprite->SetOutline(true, 0xFF000000);
            fontSprite->Draw(*uiCamera, deltaTime);
            fontSprite->End();
        }
    }
}