#include "pch.h"
#include "INPC.h"

namespace Demo {
    void INPC::Init(DX9GF::GraphicsDevice* gd, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font) {
        player = p;
        fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
    }

    void INPC::Update(unsigned long long deltaTime) {
        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = this->GetWorldPosition();

        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        this->isPlayerNear = (distance <= this->INTERACTION_DISTANCE);
    }

    void INPC::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        if (isPlayerNear && fontSprite) {
            auto [x, y] = GetWorldPosition();
            fontSprite->Begin();
            fontSprite->SetText(L"E");
            fontSprite->SetScale(1.0f);
            fontSprite->SetColor(0xFFFFFFFF);
            fontSprite->SetPosition(x - fontSprite->GetWidth() / 2.f, y - 40.f);
            fontSprite->SetOutline(true, 0xFF000000);
            fontSprite->Draw(camera, deltaTime);
            fontSprite->End();
        }
    }

    void INPC::AddLine(std::wstring name, std::wstring content) {
        DialogueLine line;
        line.name = name;
        line.content = content;
        dialogueLines.push_back(line);
    }
}