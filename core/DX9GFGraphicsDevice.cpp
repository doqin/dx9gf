#include "DX9GFGraphicsDevice.h"

DX9GF::GraphicsDevice::~GraphicsDevice() {
	if (d3ddev != NULL) d3ddev->Release();
	if (backbuffer != NULL) backbuffer->Release();
}

LPDIRECT3DDEVICE9& DX9GF::GraphicsDevice::GetDevice() {
	return d3ddev;
}

LPDIRECT3DSURFACE9& DX9GF::GraphicsDevice::GetBackBuffer() {
	return backbuffer;
}

HRESULT DX9GF::GraphicsDevice::BeginDraw()
{
	return d3ddev->BeginScene();
}

HRESULT DX9GF::GraphicsDevice::EndDraw()
{
	return d3ddev->EndScene();
}

HRESULT DX9GF::GraphicsDevice::Update()
{
	return d3ddev->Present(NULL, NULL, NULL, NULL);
}
