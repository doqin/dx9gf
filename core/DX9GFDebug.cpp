#include "DX9GFDebug.h"
#include "DX9GFUtils.h"

void DX9GF::Debug::DrawGrid(GraphicsDevice* graphicsDevice, int offsetX, int offsetY, int screenWidth, int screenHeight, int spacingX, int spacingY, 
	D3DCOLOR lineColor) {
	// Horizontal lines
	for (int i = offsetY % spacingY - spacingY; i < screenHeight; i += spacingY) {
		graphicsDevice->DrawLine(offsetX % spacingX - spacingX, i, screenWidth, i, 0xFFFF0000);
	}
	// Vertical lines
	for (int i = offsetX % spacingX - spacingX; i < screenWidth; i += spacingX) {
		graphicsDevice->DrawLine(i, offsetY % spacingY - spacingY, i, screenHeight, 0xFFFF0000);
	}
}

void DX9GF::Debug::DrawGrid(GraphicsDevice* graphicsDevice, const Camera& camera, int offsetX, int offsetY, int screenWidth, int screenHeight, int spacingX, int spacingY, D3DCOLOR lineColor)
{
	auto [x, y] = DX9GF::Utils::WorldToWindowCoords(camera, offsetX, offsetY);
	DrawGrid(graphicsDevice, x, y, screenWidth, screenHeight, spacingX, spacingY, lineColor);
}
