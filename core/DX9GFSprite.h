#pragma once
#include "DX9GFGraphicsDevice.h"
#include <expected>
#include <string>

namespace DX9GF {
	class ISprite {
	protected:
		GraphicsDevice* graphicsDevice;
	public:
		ISprite(GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {}
		ISprite() {}

		virtual void Draw() = 0;
	};

	class StaticSprite : public ISprite {
	private:
		LPDIRECT3DSURFACE9 surface = NULL;
		UINT spriteWidth, spriteHeight;
		RECT rect;

		StaticSprite(GraphicsDevice* graphicsDevice, UINT spriteWidth, UINT spriteHeight);
	public:
		StaticSprite() {};
		static std::expected<StaticSprite, std::wstring> New(GraphicsDevice* graphicsDevice, UINT spriteWidth, UINT spriteHeight);

		LPDIRECT3DSURFACE9& GetSurface();

		void SetRect(const RECT& rect);

		void FillColor(D3DCOLOR color);

		void Draw() override;

	};
};