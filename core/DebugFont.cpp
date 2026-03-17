#include "DebugFont.h"

void FontDebug::Init(DX9GF::GraphicsDevice* dev)
{
    font = std::make_shared<DX9GF::Font>(dev, L"Consolas", 14);
    sprite = std::make_shared<DX9GF::FontSprite>(font.get());
}

void FontDebug::Begin()
{
    sprite->Begin();
}

void FontDebug::End()
{
    sprite->End();
}

void FontDebug::Draw(DX9GF::Camera& cam, int x, int y, const std::wstring& text)
{
    sprite->SetPosition(x, y);
    sprite->SetText(std::wstring(text));
    sprite->Draw(cam, 0);
}