#include "DX9GFRectangleTrigger.h"
#include "../DX9GFInputManager.h"
#include "../DX9GFUtils.h"
#include "../DX9GFApplication.h"

bool DX9GF::RectangleTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    auto app = DX9GF::Application::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    float worldX = GetWorldX();
    float worldY = GetWorldY();

    auto [windowX, windowY] = Utils::WorldToWindowCoords(*camera, worldX, worldY);
    return Utils::IsWithinRectangle(mouseX, mouseY, windowX, windowY, width, height);
}

float DX9GF::RectangleTrigger::GetWidth() const {
    return width;
}

float DX9GF::RectangleTrigger::GetHeight() const
{
    return height;
}
