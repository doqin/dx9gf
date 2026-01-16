#pragma once
#include "DX9GFGraphicsDevice.h"

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
		/// Initializes the game graphics device and resources.
		/// </summary>
		/// <returns>An HRESULT indicating success or failure of the initialization.</returns>
		virtual HRESULT Init();

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
};

