#include "pch.h"
#include "DX9GFFont.h"
#include <windows.h>
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

std::vector<std::wstring> DX9GF::Font::addedFonts;

void DX9GF::Font::AddFont(std::wstring path)
{
	int result = AddFontResourceExW(
		path.c_str(),
		FR_PRIVATE,
		0
	);

	if (result > 0)
	{
		addedFonts.push_back(path);
	}
}

void DX9GF::Font::RemoveFont(std::wstring path)
{
	RemoveFontResourceExW(
		path.c_str(),
		FR_PRIVATE,
		0
	);
}

void DX9GF::Font::RemoveAllFonts()
{
	for (auto& font : addedFonts)
	{
		RemoveFontResourceExW(
			font.c_str(),
			FR_PRIVATE,
			0
		);
	}

	addedFonts.clear();
}
