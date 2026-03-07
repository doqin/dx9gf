#pragma once
#include "DX9GFGraphicsDevice.h"
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
	};
}