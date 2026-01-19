#pragma once
#include "DX9GFInterfaces.h"
#include <DX9GFSprite.h>

class ExampleGame : public DX9GF::IGame
{
private:
	DX9GF::StaticSprite colorRec;
	DX9GF::StaticSprite textureRec;
	DX9GF::AnimatedSprite mario;
public:
	ExampleGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {};
	std::expected<void, std::wstring> Init() override;
	void Update() override;
	void Draw() override;
};

