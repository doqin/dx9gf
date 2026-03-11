#include "DX9GFTexture.h"
#include <DxErr.h>
#include <Windows.h>
#include <stdexcept>

namespace
{
	static void SafeRelease(IDirect3DTexture9*& tex)
	{
		if (tex != nullptr)
		{
			tex->Release();
			tex = nullptr;
		}
	}

	static std::runtime_error MakeDxError(HRESULT hr)
	{
		auto error = DXGetErrorDescription(hr);
		std::string what = std::string(error, error + wcslen(error));
		return std::runtime_error(what);
	}

	static HRSRC FindImageResource(HMODULE moduleHandle, LPCWSTR resourceName)
	{
		HRSRC hrsrc = FindResourceW(moduleHandle, resourceName, RT_RCDATA);
		if (hrsrc != nullptr) return hrsrc;
		// Common custom types used in .rc files
		hrsrc = FindResourceW(moduleHandle, resourceName, L"JPG");
		if (hrsrc != nullptr) return hrsrc;
		hrsrc = FindResourceW(moduleHandle, resourceName, L"JPEG");
		if (hrsrc != nullptr) return hrsrc;
		hrsrc = FindResourceW(moduleHandle, resourceName, L"PNG");
		return hrsrc;
	}
}

DX9GF::Texture::Texture(GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice)
{
	
}

DX9GF::Texture::~Texture()
{
	SafeRelease(texture);
}

void DX9GF::Texture::CreatePlainTexture(D3DCOLOR color, UINT width, UINT height)
{
	SafeRelease(texture);
	HRESULT result = graphicsDevice->GetDevice()->CreateTexture(
		width, height, 1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&texture,
		nullptr
	);
	if (result != S_OK) {
		throw MakeDxError(result);
	}
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	result = texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		SafeRelease(texture);
		throw MakeDxError(result);
	}

	// Fill the texture with the color
	DWORD* pixels = (DWORD*)lockedRect.pBits;
	UINT pitch = lockedRect.Pitch / sizeof(DWORD);

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			pixels[y * pitch + x] = color;
		}
	}

	// Unlock
	texture->UnlockRect(0);
	this->width = width;
	this->height = height;
}

void DX9GF::Texture::SetColor(D3DCOLOR color)
{
	if (texture == nullptr) {
		throw std::runtime_error("Texture is null");
	}
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	HRESULT result = texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		SafeRelease(texture);
		throw MakeDxError(result);
	}

	// Fill the texture with the color
	DWORD* pixels = (DWORD*)lockedRect.pBits;
	UINT pitch = lockedRect.Pitch / sizeof(DWORD);

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			pixels[y * pitch + x] = color;
		}
	}

	// Unlock
	texture->UnlockRect(0);
}

void DX9GF::Texture::LoadTexture(std::wstring filePath, UINT width, UINT height)
{
	SafeRelease(texture);
	if (graphicsDevice->GetDevice() == nullptr) {
		throw std::runtime_error("Graphics device is null");
	}
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFile(filePath.c_str(), &imageInfo);
	auto result = D3DXCreateTextureFromFileExW(
		graphicsDevice->GetDevice(),
		filePath.c_str(),
		width,
		height,
		1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		nullptr,
		nullptr,
		&texture
	);
	if (result != D3D_OK) {
		throw MakeDxError(result);
	}
	if (width == D3DX_DEFAULT && height == D3DX_DEFAULT) {
		this->width = imageInfo.Width;
		this->height = imageInfo.Height;
	}
	else {
		this->width = width;
		this->height = height;
	}
}

void DX9GF::Texture::LoadTexture(int resourceId, UINT width, UINT height)
{
	SafeRelease(texture);
	if (graphicsDevice->GetDevice() == nullptr) {
		throw std::runtime_error("Graphics device is null");
	}
	if (width == 0) width = D3DX_DEFAULT;
	if (height == 0) height = D3DX_DEFAULT;
	D3DXIMAGE_INFO imageInfo{};
	auto handle = GetModuleHandleW(nullptr);
	auto resName = MAKEINTRESOURCEW(resourceId);

	// First try the standard resource path (expects RT_RCDATA)
	HRESULT infoHr = D3DXGetImageInfoFromResourceW(handle, resName, &imageInfo);
	HRESULT createHr = D3DXCreateTextureFromResourceExW(
		graphicsDevice->GetDevice(),
		handle,
		resName,
		width,
		height,
		1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_FILTER_LINEAR,
		D3DX_FILTER_LINEAR,
		0,
		nullptr,
		nullptr,
		&texture);

	// Some JPG resources/drivers can fail when forcing a specific format; retry letting D3DX choose.
	if (FAILED(createHr))
	{
		SafeRelease(texture);
		createHr = D3DXCreateTextureFromResourceExW(
			graphicsDevice->GetDevice(),
			handle,
			resName,
			width,
			height,
			1,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED,
			D3DX_FILTER_LINEAR,
			D3DX_FILTER_LINEAR,
			0,
			nullptr,
			nullptr,
			&texture);
	}

	// If it still fails, fall back to loading the resource bytes and creating from memory.
	if (FAILED(createHr))
	{
		SafeRelease(texture);
		HRSRC hrsrc = FindImageResource(handle, resName);
		if (hrsrc == nullptr)
		{
			throw std::runtime_error("Resource not found");
		}

		HGLOBAL hglob = LoadResource(handle, hrsrc);
		if (hglob == nullptr)
		{
			throw std::runtime_error("Failed to load resource");
		}

		DWORD size = SizeofResource(handle, hrsrc);
		void* data = LockResource(hglob);
		if (data == nullptr || size == 0)
		{
			throw std::runtime_error("Failed to lock resource");
		}

		HRESULT memInfoHr = D3DXGetImageInfoFromFileInMemory(data, size, &imageInfo);
		createHr = D3DXCreateTextureFromFileInMemoryEx(
			graphicsDevice->GetDevice(),
			data,
			size,
			width,
			height,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			D3DX_FILTER_LINEAR,
			D3DX_FILTER_LINEAR,
			0,
			nullptr,
			nullptr,
			&texture);

		if (SUCCEEDED(memInfoHr))	infoHr = memInfoHr;
	}

	if (FAILED(createHr))
	{
		SafeRelease(texture);
		throw MakeDxError(createHr);
	}

	if ((width == D3DX_DEFAULT && height == D3DX_DEFAULT) && SUCCEEDED(infoHr)) {
		this->width = imageInfo.Width;
		this->height = imageInfo.Height;
	}
	else {
		this->width = width;
		this->height = height;
	}
}

IDirect3DTexture9* DX9GF::Texture::GetRawTexture()
{
	return texture;
}

UINT DX9GF::Texture::GetWidth() const
{
	return width;
}

UINT DX9GF::Texture::GetHeight() const
{
	return height;
}

std::tuple<UINT, UINT> DX9GF::Texture::GetSize() const
{
	return std::tuple<UINT, UINT>(width, height);
}

DX9GF::GraphicsDevice* DX9GF::Texture::GetGraphicsDevice()
{
	return graphicsDevice;
}
