#include "DX9GFSprite.h"
#include <DxErr.h>

DX9GF::StaticSprite::StaticSprite(GraphicsDevice* graphicsDevice, UINT spriteWidth, UINT spriteHeight) : ISprite(graphicsDevice), spriteWidth(spriteWidth), spriteHeight(spriteHeight), rect(0, spriteWidth, 0, spriteHeight) {}

std::expected<DX9GF::StaticSprite, std::wstring> DX9GF::StaticSprite::New(GraphicsDevice* graphicsDevice, UINT spriteWidth, UINT spriteHeight) {
	StaticSprite sprite(graphicsDevice, spriteWidth, spriteHeight);
	HRESULT result = graphicsDevice->GetDevice()->CreateOffscreenPlainSurface(
		spriteWidth,
		spriteHeight,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&sprite.GetSurface(),
		NULL
	);

	if (result != D3D_OK) return std::unexpected(DXGetErrorDescription(result));
	return sprite;
}

LPDIRECT3DSURFACE9& DX9GF::StaticSprite::GetSurface()
{
	return surface;
}

void DX9GF::StaticSprite::SetRect(const RECT& rect)
{
	this->rect = rect;
}

void DX9GF::StaticSprite::FillColor(D3DCOLOR color)
{
	graphicsDevice->GetDevice()->ColorFill(surface, NULL, color);
}

void DX9GF::StaticSprite::Draw()
{
	graphicsDevice->GetDevice()->StretchRect(surface, NULL, graphicsDevice->GetBackBuffer(), &rect, D3DTEXF_NONE);
}
