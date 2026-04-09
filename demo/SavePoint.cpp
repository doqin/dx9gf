#include "pch.h"
#include "SavePoint.h"
#include <cmath>

namespace Demo {
    SavePoint::SavePoint(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
        : IGameObject(tm, x, y), transformManager(tm) {
    }

    void SavePoint::Init(DX9GF::GraphicsDevice* gd, DX9GF::Camera* camera, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::SaveManager> sm, std::shared_ptr<DX9GF::Font> font) {
        player = p;
        saveManager = sm;
        fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
        this->gd = gd;

        auto [x, y] = GetWorldPosition();

        btnYes = std::make_shared<Demo::TextButton>(
            transformManager.lock(), x - 50.f, y - 10.f, 40.f, 20.f, "Co(Y)", font.get(),
            [](DX9GF::ITrigger* t) {}
        );
        btnYes->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
            if (auto smLock = this->saveManager.lock()) {
                smLock->Save("savegame.json");
                OutputDebugStringA("DA LUU GAME THANH CONG!\n");
            }
            this->isSaveMenuOpen = false;
            });
        btnYes->SetBackgroundColors(D3DXCOLOR(0, 0, 0, 0), D3DXCOLOR(0, 1, 0, 0.3f), D3DXCOLOR(0, 1, 0, 0.6f))
            ->SetTextColors(0xFF00FF00, 0xFFFFFFFF, 0xFFFFFFFF);
        btnYes->Init(camera);

        btnNo = std::make_shared<Demo::TextButton>(
            transformManager.lock(), x + 20.f, y - 10.f, 60.f, 20.f, "Khong(N)", font.get(),
            [](DX9GF::ITrigger* t) {}
        );
        btnNo->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
            this->isSaveMenuOpen = false;
            });
        btnNo->SetBackgroundColors(D3DXCOLOR(0, 0, 0, 0), D3DXCOLOR(1, 0, 0, 0.3f), D3DXCOLOR(1, 0, 0, 0.6f))
            ->SetTextColors(0xFFFF0000, 0xFFFFFFFF, 0xFFFFFFFF);
        btnNo->Init(camera);
    }

    void SavePoint::Update(unsigned long long deltaTime) {
        if (!isVisible) return;

        auto inpMan = DX9GF::InputManager::GetInstance();

        if (isSaveMenuOpen) {
            btnYes->Update(deltaTime);
            btnNo->Update(deltaTime);

            if (inpMan->KeyPress(DIK_Y)) {
                if (auto smLock = saveManager.lock()) {
                    smLock->Save("savegame.json");
                }
                isSaveMenuOpen = false;
            }
            if (inpMan->KeyPress(DIK_N)) {
                isSaveMenuOpen = false;
            }
            return;
        }

        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = GetWorldPosition();

        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        isPlayerNear = (distance <= INTERACTION_DISTANCE);

        if (isPlayerNear && inpMan->KeyPress(DIK_E)) {
            isSaveMenuOpen = true;
        }
    }

    void SavePoint::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        if (!isVisible) return;

        auto [x, y] = GetWorldPosition();

        if (gd) {
            gd->DrawRectangle(camera, x - 15.f, y - 15.f, 30.f, 30.f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), true);
        }

        if (fontSprite) {
            fontSprite->Begin();
            if (isSaveMenuOpen) {
                if (gd) gd->DrawRectangle(camera, x - 120.f, y - 60.f, 240.f, 80.f, D3DXCOLOR(0, 0, 0, 0.9f), true);
                fontSprite->SetColor(0xFFFFFFFF);
                fontSprite->SetPosition(x - 100.f, y - 40.f);
                fontSprite->SetText(L"Ban co muon Luu Game?");
                fontSprite->Draw(camera, deltaTime);
                fontSprite->End();

                btnYes->Draw(gd, deltaTime);
                btnNo->Draw(gd, deltaTime);
            }
            else if (isPlayerNear) {
                fontSprite->SetColor(0xFF000000);
                fontSprite->SetPosition(x - 60.f, y - 30.f);
                fontSprite->SetText(L"Nhan E de Luu");
                fontSprite->Draw(camera, deltaTime);
                fontSprite->End();
            }
            else {
                fontSprite->End();
            }
        }
    }
}