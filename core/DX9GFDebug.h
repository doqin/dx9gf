#pragma once
#include "DX9GFGraphicsDevice.h"

namespace DX9GF {
	class Debug final {
	private:
		Debug() {}
		~Debug() {}
	public:
		static void DrawGrid(GraphicsDevice* graphicsDevice, float offsetX, float offsetY, float screenWidth, float screenHeight, float spacingX, float spacingY, D3DCOLOR lineColor);
	};
}