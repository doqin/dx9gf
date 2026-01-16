#pragma once
#include <d3d9.h>

namespace DX9GF {
	class IGame
	{
	private:
		LPDIRECT3D9 d3d = NULL; // Biến "tổng" của toàn bộ thư viện Direct3D
		LPDIRECT3DDEVICE9 d3ddev = NULL; // Đại diện cho card đồ họa máy tính
		LPDIRECT3DSURFACE9 backbuffer = NULL;
	protected:
		const UINT SCREEN_WIDTH;
		const UINT SCREEN_HEIGHT;
		HWND hwnd = NULL;
	public:

		IGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : hwnd(hwnd), SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight) {}
		
		~IGame();

		virtual HRESULT Init();
		virtual void Update() = 0;
		virtual void Draw() = 0;
		virtual void Dispose();
	};
};

