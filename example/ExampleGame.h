#pragma once
#include "DX9GFInterfaces.h"
#include <DX9GFSprite.h>

class ExampleGame : public DX9GF::IGame
{
private:
	DX9GF::StaticSprite obj1;
public:
	ExampleGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {};
	HRESULT Init() override;
	void Update() override;
	void Draw() override;
};

