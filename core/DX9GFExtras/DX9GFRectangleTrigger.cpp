#include "DX9GFRectangleTrigger.h"
#include "../DX9GFInputManager.h"

bool DX9GF::RectangleTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    float worldX = GetWorldX();
    float worldY = GetWorldY();
    return mouseX > worldX
        && mouseX < worldX + width
        && mouseY > worldY
        && mouseY < worldY + height;
}
