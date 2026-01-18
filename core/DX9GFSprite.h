#pragma once
#include "DX9GFInterfaces.h"
#include <expected>
#include <string>
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"

namespace DX9GF {
	class StaticSprite : public ISprite {
	private:
		RECT* p_src;
		typedef D3DXVECTOR3 Vec3;

		ID3DXSprite* p_sprite = nullptr;
		IDirect3DTexture9* p_texture = nullptr;
		D3DCOLOR color = 0xFFFFFFFF;
		Vec3 pos;

		UINT width, height;

		StaticSprite(GraphicsDevice* graphicsDevice);
	public:
		StaticSprite() {};
		~StaticSprite();
		StaticSprite(StaticSprite&& other) noexcept;
		StaticSprite& operator=(StaticSprite&& other) noexcept; // for obj1 = std::move(obj2)

		/// <summary>
		/// Creates a new StaticSprite with the specified dimensions.
		/// </summary>
		/// <param name="graphicsDevice">A pointer to the graphics device used to create the sprite.</param>
		/// <returns>A StaticSprite object if successful, or an error message if creation fails.</returns>
		static std::expected<StaticSprite, std::wstring> New(GraphicsDevice* graphicsDevice);

		/// <summary>
		/// Creates a plain texture with the specified color and dimensions.
		/// </summary>
		/// <param name="color">The color to fill the texture with.</param>
		/// <param name="width">The width of the texture in pixels.</param>
		/// <param name="height">The height of the texture in pixels.</param>
		/// <returns>An HRESULT indicating success or failure of the operation.</returns>
		HRESULT CreatePlainTexture(D3DCOLOR color, UINT width, UINT height);

		HRESULT SetColor(D3DCOLOR color);

		/// <summary>
		/// Load sprite with a texture
		/// </summary>
		/// <param name="filePath">Path to the texture file</param>
		/// <returns>An HRESULT of the load</returns>
		HRESULT LoadTexture(std::wstring filePath, UINT width, UINT height);

		/// <summary>
		/// Draws the object.
		/// </summary>
		void Draw() override;

		/// <summary>
		/// Translates the current position by the specified x and y offsets.
		/// </summary>
		/// <param name="x">The horizontal offset to translate by.</param>
		/// <param name="y">The vertical offset to translate by.</param>
		void Translate(float x, float y);

		void SetPosition(float x, float y);

		void SetSrcRect(RECT srcRect);
	};
};