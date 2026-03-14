#include "pch.h"
#include "DX9GFFont.h"
#include <DxErr.h>
#include <stdexcept>

DX9GF::Font::Font(GraphicsDevice* graphicsDevice, std::wstring fontName, INT height, INT width, UINT weight, UINT mipLevels, BOOL italic, DWORD charSet, DWORD outputPrecision, DWORD quality, DWORD pitchAndFamily) : graphicsDevice(graphicsDevice)
{
	auto result = D3DXCreateFont(graphicsDevice->GetDevice(), height, width, weight, mipLevels, italic, charSet, outputPrecision, quality, pitchAndFamily, fontName.c_str(), &font);
	if (FAILED(result)) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::invalid_argument(what);
	}
}

DX9GF::Font::~Font()
{
	if (font != nullptr) font->Release();
}

ID3DXFont* DX9GF::Font::GetRawFont()
{
	return font;
}

DX9GF::GraphicsDevice* DX9GF::Font::GetGraphicsDevice()
{
	return graphicsDevice;
}
