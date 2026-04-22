#pragma once
#include "pch.h"
#include "DX9GF.h"      
#include "DX9GFExtras.h"  
#include "Player.h"
#include "DialogueLine.h"
#include "IConversation.h"

namespace Demo {
    class NPC1 : public DX9GF::IGameObject {
    private:
        bool isConversationOpen = false;
        int currentLineIndex = 0;
        DX9GF::GraphicsDevice* gd;
        std::weak_ptr<DX9GF::TransformManager> transformManager;
        std::weak_ptr<Player> player;

        std::shared_ptr<DX9GF::AnimatedSprite> sprite;
        std::shared_ptr<DX9GF::Texture> spritesheet;
        std::shared_ptr<DX9GF::RectangleCollider> collider;
        std::shared_ptr<DX9GF::FontSprite> fontSprite;

        std::vector<DialogueLine> dialogueLines;
        bool isPlayerNear = false;
        const float INTERACTION_DISTANCE = 40.0f;

    public:
        NPC1(std::weak_ptr<DX9GF::TransformManager> tm, float x, float y);

        void Init(DX9GF::GraphicsDevice* gd, std::shared_ptr<Player> p, std::shared_ptr<DX9GF::ColliderManager> cm, std::shared_ptr<DX9GF::Font> font);
        void AddLine(std::wstring name, std::wstring content);

        void Update(unsigned long long deltaTime);
        void Draw(const DX9GF::Camera& camera, unsigned long long deltaTime);

        std::vector<DialogueLine> GetDialogueLines() { return dialogueLines; }
        bool CanInteract() const { return isPlayerNear; }
    };
}