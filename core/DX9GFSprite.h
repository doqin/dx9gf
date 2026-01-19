#pragma once
#include "DX9GFInterfaces.h"
#include <expected>
#include <string>
#include <vector>

namespace DX9GF {
	class StaticSprite : public ISprite {
	private:
		RECT* p_src;
		IDirect3DTexture9* p_texture = nullptr;
		
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
		/// Creates a plain texture with a solid color and specified dimensions.
		/// </summary>
		/// <param name="color">The solid color for the texture.</param>
		/// <param name="width">The width of the texture in pixels.</param>
		/// <param name="height">The height of the texture in pixels.</param>
		/// <returns>Returns void on success, or an error message string on failure.</returns>
		std::expected<void, std::wstring> CreatePlainTexture(D3DCOLOR color, UINT width, UINT height);

		/// <summary>
		/// Sets the current color.
		/// </summary>
		/// <param name="color">The Direct3D color value to set.</param>
		/// <returns>An expected object containing void on success, or an error message on failure.</returns>
		std::expected<void, std::wstring> SetColor(D3DCOLOR color);

		/// <summary>
		/// Loads a texture from a file with the specified dimensions.
		/// </summary>
		/// <param name="filePath">The path to the texture file to load.</param>
		/// <param name="width">The width of the texture in pixels.</param>
		/// <param name="height">The height of the texture in pixels.</param>
		/// <returns>An expected value that is empty on success, or contains an error message on failure.</returns>
		std::expected<void, std::wstring> LoadTexture(std::wstring filePath, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT);

		/// <summary>
		/// Draws the object.
		/// </summary>
		/// <returns>An expected value that is empty on success, or contains an error message on failure.</returns>
		std::expected<void, std::wstring> Draw() override;

		/// <summary>
		/// Sets the source rectangle.
		/// </summary>
		/// <param name="srcRect">The source rectangle to set.</param>
		void SetSrcRect(RECT srcRect);
	};

	class AnimatedSprite : public ISprite {
	private:
		std::vector<RECT> srcs;
		IDirect3DTexture9* p_texture = nullptr;

		UINT frame_index = 0;

		AnimatedSprite(GraphicsDevice* graphicsDevice);
	public:
		AnimatedSprite() {};
		~AnimatedSprite();
		AnimatedSprite(AnimatedSprite&& other) noexcept;
		AnimatedSprite& operator=(AnimatedSprite&& other) noexcept;

		/// <summary>
		/// Creates a new AnimatedSprite instance.
		/// </summary>
		/// <param name="graphicsDevice">A pointer to the graphics device used to create the sprite.</param>
		/// <returns>An AnimatedSprite on success, or an error message string on failure.</returns>
		static std::expected<AnimatedSprite, std::wstring> New(GraphicsDevice* graphicsDevice);

		std::expected<void, std::wstring> LoadSpriteSheet(std::wstring filePath, std::vector<RECT> frames, UINT width = D3DX_DEFAULT, UINT height = D3DX_DEFAULT);

		std::expected<void, std::wstring> Draw() override;
	};
};