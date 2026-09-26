#pragma once
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\d3dx9.h"
#include "DX9GFGraphicsDevice.h"
#include "DX9GFIDeviceLostAware.h"
#include <string>
namespace DX9GF {
	class Texture : public IDeviceLostAware {
	private:
		GraphicsDevice* graphicsDevice;
		IDirect3DTexture9* texture = nullptr;
		UINT width = 0, height = 0;
		// Only a render target (D3DPOOL_DEFAULT) needs to be released before Reset and
		// recreated after - a loaded/managed-pool texture survives Reset on its own.
		bool isRenderTarget = false;
		bool isRegisteredVolatile = false;
	public:
		Texture(GraphicsDevice* graphicsDevice);
		~Texture();
		void OnLostDevice() override;
		void OnResetDevice() override;
		void CreatePlainTexture(D3DCOLOR color, UINT width, UINT height);
		void SetColor(D3DCOLOR color);
		void LoadTexture(std::wstring filePath, UINT width = D3DX_DEFAULT_NONPOW2, UINT height = D3DX_DEFAULT_NONPOW2);
		void LoadTexture(int resourceId, UINT width = D3DX_DEFAULT_NONPOW2, UINT height = D3DX_DEFAULT_NONPOW2);
		void CaptureCurrentBackBuffer();
		IDirect3DTexture9* GetRawTexture();
		UINT GetWidth() const;
		UINT GetHeight() const;
		std::tuple<UINT, UINT> GetSize() const;
		GraphicsDevice* GetGraphicsDevice();
		void CreateRenderTarget(UINT width, UINT height);
		IDirect3DSurface9* GetSurface();
		void ReleaseRawTexture();

	};
}