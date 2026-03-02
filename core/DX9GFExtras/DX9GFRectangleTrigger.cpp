#include "DX9GFRectangleTrigger.h"
#include "../DX9GFInputManager.h"

bool DX9GF::RectangleTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    return mouseX > absoluteX
        && mouseX < absoluteX + width
        && mouseY > absoluteY
        && mouseY < absoluteY + height;
}
