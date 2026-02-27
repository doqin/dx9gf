#pragma once
#include <string>
#include <vector>
#include "DX9GFISprite.h"

namespace DX9GF {

	class StaticSprite : public ISprite {
	private:
		RECT* p_src = nullptr;
		IDirect3DTexture9* p_texture = nullptr;
		UINT width, height;
	public:
		StaticSprite(GraphicsDevice* graphicsDevice);
		~StaticSprite();

		void CreatePlainTexture(D3DCOLOR color, UINT width, UINT height);
		void SetColor(D3DCOLOR color);
		void LoadTexture(std::wstring filePath, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT);
		void Draw() override;
		void SetSrcRect(RECT srcRect);
	};

	class AnimatedSprite : public ISprite {
	private:
		std::vector<RECT> srcs;
		IDirect3DTexture9* p_texture = nullptr;

		UINT frame_index = 0;
	public:
		AnimatedSprite(GraphicsDevice* graphicsDevice);
		~AnimatedSprite();

		void LoadSpriteSheet(std::wstring filePath, std::vector<RECT> frames, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT);
		void Draw() override;
	};
};