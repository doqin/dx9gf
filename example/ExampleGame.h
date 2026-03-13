#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"

class ExampleGame : public DX9GF::IGame
{
private:
public:
	ExampleGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {};
	void Init() override;
};

