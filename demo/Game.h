#pragma once
#include "DX9GF.h"

namespace Demo {
	class Game : public DX9GF::IGame {
	public:
	Game(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {}
	void Init() override;
	};
}