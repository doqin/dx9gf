#include "DX9GFEllipseTrigger.h"
#include "../DX9GFInputManager.h"

bool DX9GF::EllipseTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    float a = width / 2;
    float b = height / 2;
    float worldX = GetWorldX();
    float worldY = GetWorldY();
    return pow(mouseX - (worldX + a), 2) / pow(a, 2) + pow(mouseY - (worldY + b), 2) / pow(b, 2) < 1;
}
