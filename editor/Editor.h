#pragma once
#include "DX9GF.h"

namespace Editor {
	class Editor : public DX9GF::IGame {
	public:
		Editor(HWND hwnd, UINT screenWidth, UINT screenHeight) : IGame(hwnd, screenWidth, screenHeight) {}
		void Init() override;
		void OnResize(UINT width, UINT height) override;
		void Dispose() override;
	};
}