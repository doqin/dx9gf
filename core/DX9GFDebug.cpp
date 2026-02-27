#include "DX9GFDebug.h"

void DX9GF::Debug::DrawGrid(
	GraphicsDevice* graphicsDevice, 
	float offsetX, 
	float offsetY, 
	float screenWidth, 
	float screenHeight, 
	float spacingX, 
	float spacingY, 
	D3DCOLOR lineColor
) {
	for (int i = offsetY; i < screenHeight; i += spacingY) {
		graphicsDevice->DrawLine(offsetX, i, screenWidth, i, 0xFFFF0000);
	}
	for (int i = offsetX; i < screenWidth; i += spacingX) {
		graphicsDevice->DrawLine(i, offsetY, i, screenHeight, 0xFFFF0000);
	}
}
