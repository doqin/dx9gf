#pragma once
#include <d3d9.h>

namespace DX9GF {
	class GraphicsDevice {
	private:
		LPDIRECT3DDEVICE9 d3ddev = NULL; // Đại diện cho card đồ họa máy tính
		LPDIRECT3DSURFACE9 backbuffer = NULL;
	public:
		GraphicsDevice() {};
		GraphicsDevice(LPDIRECT3DDEVICE9 d3ddev, LPDIRECT3DSURFACE9 backbuffer) : d3ddev(d3ddev), backbuffer(backbuffer) {};
		~GraphicsDevice();

		LPDIRECT3DDEVICE9& GetDevice();
		LPDIRECT3DSURFACE9& GetBackBuffer();

		HRESULT BeginDraw();
		HRESULT EndDraw();
		HRESULT Update();
	};
};