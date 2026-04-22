#include "pch.h"
#include "NPC1.h"

namespace Demo {
    NPC1::NPC1(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y)
        : IGameObject(tm, x, y), transformManager(tm) {
    }

    void NPC1::Init(DX9GF::GraphicsDevice* gd, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font) {
        player = p;
        fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());

        collider = std::make_shared<DX9GF::RectangleCollider>(transformManager, 32.f, 32.f, this->GetWorldX(), this->GetWorldY());
        collider->SetOriginCenter();
        cm->Add(collider);

        spritesheet = std::make_shared<DX9GF::Texture>(gd);
        spritesheet->LoadTexture(L"NPC1.png");
        sprite = std::make_shared<DX9GF::AnimatedSprite>(spritesheet.get(), DX9GF::Utils::CreateRectsHorizontal(0, 0, 32, 32, 4), 8, true);
        sprite->SetOrigin(16.f, 16.f);
        sprite->SetPosition(this->GetWorldX(), this->GetWorldY());
    }

    void NPC1::Update(unsigned long long deltaTime) {
        auto pLock = player.lock();
        if (!pLock) return;

        auto [px, py] = pLock->GetWorldPosition();
        auto [sx, sy] = this->GetWorldPosition();

        float distance = std::sqrt((px - sx) * (px - sx) + (py - sy) * (py - sy));
        this->isPlayerNear = (distance <= this->INTERACTION_DISTANCE);
    }

    void NPC1::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
        auto [x, y] = GetWorldPosition();

        sprite->Begin();
        sprite->Draw(camera, deltaTime);
        sprite->End();

        if (isPlayerNear && fontSprite) {
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


    void NPC1::AddLine(std::wstring name, std::wstring content) {
        DialogueLine line;
        line.name = name;
        line.content = content;
        dialogueLines.push_back(line);
    }
}