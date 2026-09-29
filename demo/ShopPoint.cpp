#include "pch.h"
#include "SettingsManager.h"
#include "ShopPoint.h"
#include "CardShop.h"
#include <cmath>

namespace Demo {
	ShopPoint::ShopPoint(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
		: IGameObject(tm, x, y), transformManager(tm) {
	}

    void ShopPoint::Init(Game* game, DX9GF::GraphicsDevice* gd, DX9GF::Camera* camera, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font, std::shared_ptr<DX9GF::CommandBuffer> drawBuffer, const ShopConfig& shopConfig) {
        this->game = game;
        this->worldCamera = camera;
        player = p;
        fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
        this->drawBuffer = drawBuffer;
        this->gd = gd;
        this->config = shopConfig;
        collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, 80.f, 20.f, GetWorldX(), GetWorldY() + 30.f);
        collider->SetOriginCenter();
        cm->Add(collider);

        spritesheet = std::make_shared<DX9GF::Texture>(gd);
        spritesheet->LoadTexture(L"assets/Shop-Sheet.png");

        sprite = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 80, 80, 8), 8);
        sprite->SetOrigin(40.f, 40.f);
        sprite->SetPosition(GetWorldX(), GetWorldY());
    }

    void Demo::ShopPoint::Update(unsigned long long deltaTime) {
        if (!isVisible) return;
        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = GetWorldPosition();
        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        isPlayerNear = (distance <= INTERACTION_DISTANCE);

        auto inpMan = DX9GF::InputManager::GetInstance();

        if (isPlayerNear && inpMan->KeyPress(SettingsManager::GetInstance()->GetKeybind("INTERACT"))) {
            auto [sw, sh] = worldCamera->GetScreenResolution();

            if (config.factory) {
                auto shopScene = config.factory(game, pLock.get(), sw, sh);
                auto sceMan = game->GetSceneManager();
                sceMan->InsertScene(sceMan->GetIndex() + 1, shopScene);
                sceMan->GoToNext();
            }
        }
    }

    void Demo::ShopPoint::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        if (!isVisible) return;
        sprite->Begin();
        sprite->Draw(camera, deltaTime);
        sprite->End();
        if (fontSprite) {
            std::wstring text = (config.type == ShopType::Card) ? L"CARD" : L"ITEM";
            uint32_t textColor = 0xFFFFFFFF;

            switch (config.tier) {
            case ShopTier::BASIC:
                textColor = 0xFFFFFFFF;
                break;
            case ShopTier::HYBRID:
            case ShopTier::RK_HYBRID:
                textColor = 0xFF00FFFF;
                break;
            case ShopTier::PREMIUM:
                textColor = 0xFFFF00FF;
                break;
            }

            fontSprite->Begin();
            fontSprite->SetText(text);

            float baseW = static_cast<float>(fontSprite->GetWidth());
            float baseH = static_cast<float>(fontSprite->GetHeight());
            float scaleX = 65.0f / baseW;
            float scaleY = 14.0f / baseH;
            float scale = std::min(scaleX, scaleY);

            fontSprite->SetScale(scale);
            fontSprite->SetColor(textColor);
            fontSprite->SetOutline(true, 0xFF000000);

            auto [wx, wy] = GetWorldPosition();
            float textW = baseW * scale;
            float textH = baseH * scale;

            fontSprite->SetPosition(wx + 0.5f - (textW / 2.0f), wy - 21.0f - (textH / 2.0f));
            fontSprite->Draw(camera, deltaTime);
            fontSprite->End();
        }

        DrawPosition(deltaTime, gd, camera);
    }

    void Demo::ShopPoint::DrawUI(DX9GF::Camera* uiCamera, unsigned long long deltaTime) {
        if (!isVisible || !uiCamera || !worldCamera) return;

        if (fontSprite && isPlayerNear) {
            auto [x, y] = GetWorldPosition();
            float uiX = (x - worldCamera->GetPosition().x) * worldCamera->GetZoom();
            float uiY = (y - worldCamera->GetPosition().y) * worldCamera->GetZoom();

            float zoom = worldCamera->GetZoom();
            float scale = 1.0f * zoom;

            fontSprite->Begin();
            fontSprite->SetText(SettingsManager::GetInstance()->GetKeybindDisplayName("INTERACT"));
            fontSprite->SetScale(scale);
            fontSprite->SetColor(0xFFFFFFFF);

            float textW = fontSprite->GetWidth() * scale;
            float textH = fontSprite->GetHeight() * scale;
            fontSprite->SetPosition(uiX - textW / 2.f, uiY - 30.f * zoom - textH / 2.f);
            fontSprite->SetOutline(true, 0xFF000000);
            fontSprite->Draw(*uiCamera, deltaTime);
            fontSprite->End();
        }
    }
}