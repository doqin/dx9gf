#pragma once
#include "DX9GFGraphicsDevice.h"
#include "DX9GFFont.h"
#include "FontDebug.h"
#include <map>
#include <string>
#include <vector>

namespace DX9GF {
	class Debug final {
	private:
		Debug() {}
		~Debug() {}

		static Font* debugFont;
		static FontSprite* debugSprite;
		static std::map<int, std::wstring> coordinateCache;
	public:

		static void Init(GraphicsDevice* graphicsDevice);
		static void Release();

		static void DrawGrid(GraphicsDevice* graphicsDevice, int offsetX, int offsetY, int screenWidth, int screenHeight, int spacingX, int spacingY, D3DCOLOR lineColor);
		static void DrawGrid(GraphicsDevice* graphicsDevice, const Camera& camera, int offsetX, int offsetY, int screenWidth, int screenHeight, int spacingX, int spacingY, D3DCOLOR lineColor);

		static void DrawAxes(const Camera& camera, int range, int step, D3DCOLOR color = 0xFFFFFFFF);

		static bool drawGrid;
	};
}