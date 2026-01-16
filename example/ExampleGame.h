#pragma once
#include "DX9GFInterfaces.h"

class ExampleGame : public DX9GF::IGame
{
public:
	ExampleGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {};
	HRESULT Init() override;
	void Update() override;
	void Draw() override;
};

