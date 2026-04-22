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
    
        if (sprite) {
            sprite->Begin();
            sprite->Draw(camera, deltaTime);
            sprite->End();
        }

        if (!fontSprite || !this->gd) return;
        fontSprite->Begin();

        if (isConversationOpen && !dialogueLines.empty()) {

            float screenW = 800.f;
            float screenH = 600.f;

            float boxW = screenW - 40.f;
            float boxH = 120.f;
            float boxX = 20.f; 
            float boxY = screenH - boxH - 20.f; 
            float nameBoxW = 120.f;
            float nameBoxH = 30.f;
            float nameBoxX = boxX;
            float nameBoxY = boxY - nameBoxH;

            this->gd->DrawRectangle(camera, nameBoxX, nameBoxY, nameBoxW, nameBoxH, 0xFFE0E0E0, true);
            this->gd->DrawRectangle(camera, nameBoxX, nameBoxY, nameBoxW, nameBoxH, 0xFF000000, false);

            this->gd->DrawRectangle(camera, boxX, boxY, boxW, boxH, 0xFFE0E0E0, true);
            this->gd->DrawRectangle(camera, boxX, boxY, boxW, boxH, 0xFF000000, false);

            fontSprite->SetColor(0xFF000000);
            fontSprite->SetOutline(false);

            std::wstring nameStr = dialogueLines[currentLineIndex].name;
            fontSprite->SetScale(0.5f);
            fontSprite->SetText(std::move(nameStr));
            fontSprite->SetPosition(nameBoxX + 10.f, nameBoxY + 5.f);
            fontSprite->Draw(camera, deltaTime);

            std::wstring contentStr = dialogueLines[currentLineIndex].content;
            fontSprite->SetScale(0.6f);
            fontSprite->SetText(std::move(contentStr));
            fontSprite->SetPosition(boxX + 20.f, boxY + 25.f);
            fontSprite->Draw(camera, deltaTime);
        }
        else if (isPlayerNear) {
            auto [x, y] = GetWorldPosition();
            std::wstring hint = L"E";
            fontSprite->SetText(std::move(hint));
            fontSprite->SetScale(1.f);
            fontSprite->SetColor(0xFFFFFFFF);
            fontSprite->SetOutline(true, 0xFF000000);
            fontSprite->SetPosition(x - fontSprite->GetWidth() / 2.f, y - 50.f);
            fontSprite->Draw(camera, deltaTime);
        }

        fontSprite->End();
    }

    void NPC1::AddLine(std::wstring name, std::wstring content) {
        DialogueLine line;
        line.name = name;
        line.content = content;
        dialogueLines.push_back(line);
    }
}