#pragma once
#include "DX9GF.h"
#include <memory>

class FontDebug
{
private:
    std::shared_ptr<DX9GF::Font> font;
    std::shared_ptr<DX9GF::FontSprite> sprite;

public:
    void Init(DX9GF::GraphicsDevice* dev);
	void Begin();
	void End();
    void Draw(DX9GF::Camera& cam, int x, int y, const std::wstring& text);
};