#include "DX9GFSprite.h"
#include <DxErr.h>
#include <d3dx9.h>

DX9GF::StaticSprite::StaticSprite(GraphicsDevice* graphicsDevice) 
	: ISprite(graphicsDevice) {}

DX9GF::StaticSprite::~StaticSprite()
{
	if (p_texture != nullptr) p_texture->Release();
	if (p_sprite != nullptr) p_sprite->Release();
	delete p_src;
}

DX9GF::StaticSprite::StaticSprite(StaticSprite&& other) noexcept
	: ISprite(other.graphicsDevice)
{
	p_sprite = other.p_sprite;
	p_texture = other.p_texture;
	color = other.color;
	pos = other.pos;
	p_src = other.p_src;

	other.p_sprite = nullptr;
	other.p_texture = nullptr;
}

DX9GF::StaticSprite& DX9GF::StaticSprite::operator=(StaticSprite&& other) noexcept
{
	if (this != &other)
	{
		if (p_texture != nullptr) p_texture->Release();
		if (p_sprite != nullptr) p_sprite->Release();

		graphicsDevice = other.graphicsDevice;
		p_sprite = other.p_sprite;
		p_texture = other.p_texture;
		color = other.color;
		pos = other.pos;
		p_src = other.p_src;

		other.p_sprite = nullptr;
		other.p_texture = nullptr;
	}
	return *this;
}

std::expected<DX9GF::StaticSprite, std::wstring>
DX9GF::StaticSprite::New(
	GraphicsDevice* graphicsDevice
)
{
	StaticSprite sprite(graphicsDevice);
	HRESULT result = D3DXCreateSprite(graphicsDevice->GetDevice(), &sprite.p_sprite);

	if (result != D3D_OK) return std::unexpected(DXGetErrorDescription(result));
	return sprite;
}

std::expected<void, std::wstring> DX9GF::StaticSprite::CreatePlainTexture(D3DCOLOR color, UINT width, UINT height)
{
	if (p_texture != nullptr) p_texture->Release();
	HRESULT result = graphicsDevice->GetDevice()->CreateTexture(
		width, height, 1,
		0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&p_texture,
		nullptr
	);
	if (result != S_OK) return std::unexpected(DXGetErrorDescription(result));
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	result = p_texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		p_texture->Release();
		return std::unexpected(DXGetErrorDescription(result));
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
	p_texture->UnlockRect(0);
	this->width = width;
	this->height = height;

	return {};
}

std::expected<void, std::wstring> DX9GF::StaticSprite::SetColor(D3DCOLOR color)
{
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	HRESULT result = p_texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		p_texture->Release();
		return std::unexpected(DXGetErrorDescription(result));
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
	p_texture->UnlockRect(0);
}

std::expected<void, std::wstring>
DX9GF::StaticSprite::LoadTexture(
	std::wstring filePath, 
	UINT width, 
	UINT height
)
{
	if (this->p_texture != nullptr) p_texture->Release();
	if (graphicsDevice->GetDevice() == nullptr) return std::unexpected(L"Graphics device is null");
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
		&p_texture
	);
	if (result != D3D_OK) return std::unexpected(DXGetErrorDescription(result));
	return {};
}

std::expected<void, std::wstring>
DX9GF::StaticSprite::Draw()
{
	p_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	p_sprite->Draw(
		p_texture,
		p_src,
		nullptr,
		&pos,
		color
	);
	p_sprite->End();
	return {};
}

void 
DX9GF::StaticSprite::SetSrcRect(
	RECT srcRect
)
{
	if (this->p_src != nullptr) delete p_src;
	this->p_src = new RECT(srcRect);
}

DX9GF::AnimatedSprite::AnimatedSprite(GraphicsDevice* graphicsDevice) : ISprite(graphicsDevice) {}

DX9GF::AnimatedSprite::~AnimatedSprite()
{
	if (p_texture != nullptr) p_texture->Release();
	if (p_sprite != nullptr) p_sprite->Release();
}

DX9GF::AnimatedSprite::AnimatedSprite(AnimatedSprite&& other) noexcept : ISprite(other.graphicsDevice)
{
	p_sprite = other.p_sprite;
	p_texture = other.p_texture;
	color = other.color;
	pos = other.pos;
	srcs = std::move(other.srcs);

	other.p_sprite = nullptr;
	other.p_texture = nullptr;
}

DX9GF::AnimatedSprite& 
DX9GF::AnimatedSprite::operator=(AnimatedSprite&& other) noexcept
{
	if (this != &other)
	{
		if (p_texture != nullptr) p_texture->Release();
		if (p_sprite != nullptr) p_sprite->Release();

		graphicsDevice = other.graphicsDevice;
		p_sprite = other.p_sprite;
		p_texture = other.p_texture;
		color = other.color;
		pos = other.pos;
		srcs = std::move(other.srcs);

		other.p_sprite = nullptr;
		other.p_texture = nullptr;
	}
	return *this;
}

std::expected<DX9GF::AnimatedSprite, std::wstring> 
DX9GF::AnimatedSprite::New(GraphicsDevice* graphicsDevice)
{
	AnimatedSprite sprite(graphicsDevice);
	HRESULT result = D3DXCreateSprite(graphicsDevice->GetDevice(), &sprite.p_sprite);

	if (result != D3D_OK) return std::unexpected(DXGetErrorDescription(result));
	return sprite;
}

std::expected<void, std::wstring> 
DX9GF::AnimatedSprite::LoadSpriteSheet(
	std::wstring filePath, 
	std::vector<RECT> frames, 
	UINT width, 
	UINT height
)
{
	if (this->p_texture != nullptr) p_texture->Release();
	if (graphicsDevice->GetDevice() == nullptr) return std::unexpected(L"Graphics device is null");
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
		&p_texture
	);
	if (result != D3D_OK) return std::unexpected(DXGetErrorDescription(result));
	this->srcs = frames;
	return {};
}

std::expected<void, std::wstring> 
DX9GF::AnimatedSprite::Draw()
{
	if (srcs.size() == 0) return std::unexpected(L"Sprite frames are not set!");
	frame_index++;
	if (frame_index >= srcs.size()) frame_index = 0;
	try {
		auto p_src = &srcs.at(frame_index);
		p_sprite->Begin(D3DXSPRITE_ALPHABLEND);
		p_sprite->Draw(
			p_texture,
			p_src,
			nullptr,
			&pos,
			color
		);
		p_sprite->End();
		return {};
	}
	catch (std::exception e) {
		std::string what_str = e.what();
		std::wstring w_what(what_str.begin(), what_str.end());
		return std::unexpected(w_what);
	}
}
