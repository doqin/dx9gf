#pragma once
#include "DX9GFGraphicsDevice.h"
#include <vector>
#include <string>
namespace DX9GF {
	class Font {
	private:
		GraphicsDevice* graphicsDevice;
		ID3DXFont* font;
	protected:
	public:
		Font(
			GraphicsDevice* graphicsDevice, 
			std::wstring fontName, 
			INT height, 
			INT width = 0, 
			UINT weight = FW_REGULAR, 
			UINT mipLevels = 1, 
			BOOL italic = FALSE,
			DWORD charSet = DEFAULT_CHARSET,
			DWORD outputPrecision = OUT_DEFAULT_PRECIS,
			DWORD quality = ANTIALIASED_QUALITY,
			DWORD pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE
		);
		~Font();
		ID3DXFont* GetRawFont();
		GraphicsDevice* GetGraphicsDevice();

	private:
		static std::vector<std::wstring> addedFonts;
	public:
		static void AddFont(std::wstring path);
		static void RemoveFont(std::wstring path);
		static void RemoveAllFonts();


	};
}