#pragma once
#include "framework.h"
#include <vector>
#include <tuple>
#include "DX9GFCamera.h"
#include <string>

namespace DX9GF {
	class Utils final {
	public:
		/// <summary>
		/// Creates a collection of rectangular frames from a sprite sheet.
		/// </summary>
		/// <param name="sheetWidth">The width of the sprite sheet in pixels.</param>
		/// <param name="sheetHeight">The height of the sprite sheet in pixels.</param>
		/// <param name="frameWidth">The width of each individual frame in pixels.</param>
		/// <param name="frameHeight">The height of each individual frame in pixels.</param>
		/// <param name="frameCount">The number of frames to create.</param>
		/// <param name="offset">The number of frames to skip from the beginning of the sprite sheet.</param>
		/// <returns>A vector of RECT structures representing the position and size of each frame.</returns>
		static std::vector<RECT> CreateFrames(UINT sheetWidth, UINT sheetHeight, UINT frameWidth, UINT frameHeight, UINT frameCount, UINT offset = 0);
		static bool IsWithinRectangle(float pointX, float pointY, float rectX, float rectY, float rectWidth, float rectHeight);
		static void SetMousePos(int x, int y);
		static std::tuple<float, float> WindowToWorldCoords(const DX9GF::Camera& camera, float windowX, float windowY);
		static std::tuple<float, float> WorldToWindowCoords(const DX9GF::Camera& camera, float worldX, float worldY);
		static std::wstring ToWide(const char* s);
	};
};