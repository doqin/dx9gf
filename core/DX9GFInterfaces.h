#pragma once
#include "DX9GFGraphicsDevice.h"
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
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
		virtual void Init();

		/// <summary>
		/// Updates the object's state
		/// </summary>
		/// <param name="deltaTime">The time elapsed since the last frame in milliseconds</param>
		virtual void Update(ULONGLONG deltaTime) = 0;

		/// <summary>
		/// Draws the frame
		/// </summary>
		/// <param name="deltaTime">The time elapsed since the last frame in milliseconds</param>
		virtual void Draw(ULONGLONG deltaTime) = 0;

		/// <summary>
		/// Releases all resources used by the object.
		/// </summary>
		virtual void Dispose() = 0;
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

		virtual void Draw() = 0;

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

