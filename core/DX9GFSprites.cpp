#include "DX9GFSprites.h"
#include "DX9GFGraphicsDevice.h"
#include <DxErr.h>
#include <d3dx9.h>
#include <stdexcept>

DX9GF::StaticSprite::StaticSprite(GraphicsDevice* graphicsDevice) : ISprite(graphicsDevice)
{
	HRESULT result = D3DXCreateSprite(graphicsDevice->GetDevice(), &p_sprite);
	if (result != D3D_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::invalid_argument(what);
	}
}

DX9GF::StaticSprite::~StaticSprite()
{
	if (p_texture != nullptr) p_texture->Release();
	if (p_sprite != nullptr) p_sprite->Release();
	if (p_src != nullptr) delete p_src;
}

void DX9GF::StaticSprite::CreatePlainTexture(D3DCOLOR color, UINT width, UINT height)
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
	if (result != S_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	result = p_texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		p_texture->Release();
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
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
}

void DX9GF::StaticSprite::SetColor(D3DCOLOR color)
{
	// Lock the texture
	D3DLOCKED_RECT lockedRect;
	HRESULT result = p_texture->LockRect(0, &lockedRect, nullptr, 0);
	if (FAILED(result))
	{
		p_texture->Release();
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
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

void DX9GF::StaticSprite::LoadTexture(
	std::wstring filePath, 
	UINT width, 
	UINT height
)
{
	if (this->p_texture != nullptr) p_texture->Release();
	if (graphicsDevice->GetDevice() == nullptr) {
		throw std::runtime_error("Graphics device is null");
	}
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
	if (result != D3D_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

void DX9GF::StaticSprite::Begin()
{
	p_sprite->OnLostDevice();
	p_sprite->OnResetDevice();
	if (auto result = p_sprite->Begin(D3DXSPRITE_ALPHABLEND); FAILED(result)) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

void DX9GF::StaticSprite::Draw(const Camera& camera, unsigned long long deltaTime)
{
	auto matWorld = camera.GetTransformMatrix();
	p_sprite->SetTransform(&matWorld);
	p_sprite->Draw(
		p_texture,
		p_src,
		nullptr,
		&pos,
		color
	);
}

void DX9GF::StaticSprite::End()
{
	p_sprite->End();
}

void DX9GF::StaticSprite::SetSrcRect(RECT srcRect)
{
	if (this->p_src != nullptr) delete p_src;
	this->p_src = new RECT(srcRect);
}

DX9GF::AnimatedSprite::AnimatedSprite(GraphicsDevice* graphicsDevice) : ISprite(graphicsDevice) {
	HRESULT result = D3DXCreateSprite(graphicsDevice->GetDevice(), &p_sprite);

	if (result != D3D_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

DX9GF::AnimatedSprite::~AnimatedSprite()
{
	if (p_texture != nullptr) p_texture->Release();
	if (p_sprite != nullptr) p_sprite->Release();
}

void DX9GF::AnimatedSprite::SetFrameRate(unsigned int frameRate)
{
	this->frameRate = frameRate;
}

void DX9GF::AnimatedSprite::LoadSpriteSheet(std::wstring filePath,
	std::vector<RECT> frames,
	UINT width,
	UINT height,
	UINT frameRate)
{
	if (this->p_texture != nullptr) p_texture->Release();
	if (graphicsDevice->GetDevice() == nullptr) throw std::runtime_error("Graphics device is null");
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
	if (result != D3D_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
	this->srcs = frames;
	this->frameRate = frameRate;
}

void DX9GF::AnimatedSprite::Begin()
{
	p_sprite->OnLostDevice();
	p_sprite->OnResetDevice();
	if (auto result = p_sprite->Begin(D3DXSPRITE_ALPHABLEND); FAILED(result)) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

void DX9GF::AnimatedSprite::Draw(const Camera& camera, unsigned long long deltaTime)
{
	if (srcs.size() == 0) throw std::runtime_error("Sprite frames are not set!");
	delta += deltaTime;
	if (delta > 1000 / frameRate) {
		frame_index += delta / (1000 / frameRate); // this will handle skipping more than 1 frame if delta is worth 2+ frame time
		delta = 0;
	}
	frame_index %= srcs.size();
	auto p_src = &srcs.at(frame_index);
	auto matWorld = camera.GetTransformMatrix();
	p_sprite->SetTransform(&matWorld);
	p_sprite->Draw(
		p_texture,
		p_src,
		nullptr,
		&pos,
		color
	);
}

void DX9GF::AnimatedSprite::End()
{
	p_sprite->End();
}

DX9GF::FontSprite::FontSprite(Font* font) : ISprite(font->GetGraphicsDevice()), font(font)
{
	HRESULT result = D3DXCreateSprite(graphicsDevice->GetDevice(), &p_sprite);
	if (result != D3D_OK) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

DX9GF::FontSprite::~FontSprite()
{
	if (p_src != nullptr) delete p_src;
	if (p_sprite != nullptr) delete p_sprite;
}

void DX9GF::FontSprite::SetText(std::wstring&& text)
{
	this->text = text;
}

void DX9GF::FontSprite::SetColor(D3DCOLOR color)
{
	this->color = color;
}

void DX9GF::FontSprite::Begin()
{
	p_sprite->OnLostDevice();
	p_sprite->OnResetDevice();
	font->GetRawFont()->OnLostDevice();
	font->GetRawFont()->OnResetDevice();
	if (auto result = p_sprite->Begin(D3DXSPRITE_ALPHABLEND); FAILED(result)) {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error(what);
	}
}

void DX9GF::FontSprite::Draw(const Camera& camera, unsigned long long deltaTime)
{
	auto matWorld = camera.GetTransformMatrix();
	p_sprite->SetTransform(&matWorld);
	RECT rect{};
	if (p_src == nullptr) {
		rect.left = pos.x;
		rect.top = pos.y;
		rect.right = pos.x;
		rect.bottom = pos.y;
	}
	else {
		rect = *p_src;
	}
	font->GetRawFont()->DrawText(p_sprite, text.c_str(), -1, &rect, DT_NOCLIP, color);
}

void DX9GF::FontSprite::End()
{
	p_sprite->End();
}
