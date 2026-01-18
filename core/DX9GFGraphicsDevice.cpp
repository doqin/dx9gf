#include "DX9GFGraphicsDevice.h"

DX9GF::GraphicsDevice::GraphicsDevice(IDirect3DDevice9* d3ddev, IDirect3DSurface9* backbuffer) : d3ddev(d3ddev), backbuffer(backbuffer)
{
	d3ddev->GetViewport(&viewport);
}

DX9GF::GraphicsDevice::~GraphicsDevice() {
	if (d3ddev != NULL) d3ddev->Release();
	if (backbuffer != NULL) backbuffer->Release();
}

IDirect3DDevice9*& DX9GF::GraphicsDevice::GetDevice() {
	return d3ddev;
}

LPDIRECT3DSURFACE9& DX9GF::GraphicsDevice::GetBackBuffer() {
	return backbuffer;
}

HRESULT DX9GF::GraphicsDevice::SetViewport(D3DVIEWPORT9 viewport)
{
	return d3ddev->SetViewport(&viewport);
}

HRESULT DX9GF::GraphicsDevice::SetViewport(DWORD x, DWORD y, DWORD width, DWORD height, float minZ, float maxZ)
{
	D3DVIEWPORT9 vp = { x, y, width, height, minZ, maxZ };
	return d3ddev->SetViewport(&vp);
}

HRESULT DX9GF::GraphicsDevice::BeginDraw()
{
	return d3ddev->BeginScene();
}

HRESULT DX9GF::GraphicsDevice::EndDraw()
{
	return d3ddev->EndScene();
}

HRESULT DX9GF::GraphicsDevice::Present()
{
	return d3ddev->Present(NULL, NULL, NULL, NULL);
}

HRESULT DX9GF::GraphicsDevice::IsValid()
{
	if (d3ddev == NULL)
		return E_FAIL;
	
	return d3ddev->TestCooperativeLevel();
}
