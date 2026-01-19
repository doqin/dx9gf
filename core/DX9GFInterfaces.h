#pragma once
#include "DX9GFGraphicsDevice.h"
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#include <expected>
#include <string>

namespace DX9GF {
	class IGame
	{
	private:
		LPDIRECT3D9 d3d = NULL; // Biến "tổng" của toàn bộ thư viện Direct3D
		HWND hwnd = NULL;
	protected:
		const UINT SCREEN_WIDTH;
		const UINT SCREEN_HEIGHT;

		GraphicsDevice graphicsDevice;
		
		HWND GetHwnd() const;

	public:

		IGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : hwnd(hwnd), SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight) {}
		
		~IGame();

		/// <summary>
		/// Initializes the object.
		/// </summary>
		/// <returns>An expected value containing void on success, or a wide string error message on failure.</returns>
		virtual std::expected<void, std::wstring> Init();

		/// <summary>
		/// Updates the game's state.
		/// </summary>
		virtual void Update() = 0;

		/// <summary>
		/// Draws the object. This pure virtual function must be implemented by derived classes.
		/// </summary>
		virtual void Draw() = 0;

		/// <summary>
		/// Releases all resources used by the object.
		/// </summary>
		virtual void Dispose();
	};

	class ISprite {
	protected:
		GraphicsDevice* graphicsDevice;
		ID3DXSprite* p_sprite = nullptr;
		D3DXVECTOR3 pos;
		D3DCOLOR color = 0xFFFFFFFF;
	public:
		ISprite(GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {}
		ISprite() {}

		virtual std::expected<void, std::wstring> Draw() = 0;

		/// <summary>
		/// Translates the current position by the specified x and y offsets.
		/// </summary>
		/// <param name="x">The horizontal offset to translate by.</param>
		/// <param name="y">The vertical offset to translate by.</param>
		void Translate(float x, float y);

		/// <summary>
		/// Sets the position of the sprite
		/// </summary>
		/// <param name="x">The position in the horizontal coordinate</param>
		/// <param name="y">The position in the vertical coordinate</param>
		void SetPosition(float x, float y);
	};
};

