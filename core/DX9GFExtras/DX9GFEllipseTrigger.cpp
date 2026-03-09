#include "DX9GFEllipseTrigger.h"
#include "../DX9GFInputManager.h"
#include "../DX9GFUtils.h"

bool DX9GF::EllipseTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    float worldX = GetWorldX();
    float worldY = GetWorldY();
    auto [windowX, windowY] = Utils::WorldToWindowCoords(*camera, worldX, worldY);
    float a = width / 2;
    float b = height / 2;
    
    return pow(mouseX - (windowX + a), 2) / pow(a, 2) + pow(mouseY - (windowY + b), 2) / pow(b, 2) < 1;
}
