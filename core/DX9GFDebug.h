#pragma once
#include "DX9GFGraphicsDevice.h"

namespace DX9GF {
	class Debug final {
	private:
		Debug() {}
		~Debug() {}
	public:
		static void DrawGrid(GraphicsDevice* graphicsDevice, int offsetX, int offsetY, int screenWidth, int screenHeight, int spacingX, int spacingY, D3DCOLOR lineColor);
	};
}