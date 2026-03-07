#pragma once
#include <string>
#include <vector>
#include "DX9GFISprite.h"
#include "DX9GFFont.h"

namespace DX9GF {

	class StaticSprite : public ISprite {
	private:
		RECT* p_src = nullptr;
		IDirect3DTexture9* p_texture = nullptr;
		UINT width = 0, height = 0;
	public:
		StaticSprite(GraphicsDevice* graphicsDevice);
		~StaticSprite();

		void CreatePlainTexture(D3DCOLOR color, UINT width, UINT height);
		void SetColor(D3DCOLOR color);
		void LoadTexture(std::wstring filePath, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT);
		void Begin() override;
		void Draw(const Camera& camera, unsigned long long deltaTime) override;
		void End() override;
		void SetSrcRect(RECT srcRect);
	};

	class AnimatedSprite : public ISprite {
	private:
		std::vector<RECT> srcs;
		IDirect3DTexture9* p_texture = nullptr;

		UINT frame_index = 0;
		unsigned int frameRate = 24;
		unsigned long long delta = 0;
	public:
		AnimatedSprite(GraphicsDevice* graphicsDevice);
		~AnimatedSprite();

		void SetFrameRate(unsigned int frameRate);
		void LoadSpriteSheet(std::wstring filePath, std::vector<RECT> frames, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT, UINT frameRate = 24);
		void Begin() override;
		void Draw(const Camera& camera, unsigned long long deltaTime) override;
		void End() override;
	};

	class FontSprite : public ISprite {
	private:
		RECT* p_src = nullptr;
		Font* font = nullptr;
		std::wstring text;
	public:
		FontSprite(Font* font);
		~FontSprite();
		void SetText(std::wstring&& text);
		void SetColor(D3DCOLOR color);
		void Begin() override;
		void Draw(const Camera& camera, unsigned long long deltaTime) override;
		void End() override;
	};
};