#include "DX9GFGraphicsDevice.h"

struct Vertex {
	float x, y, z, rhw; // rhw is reciprocal of homogenous w
	DWORD color;
};

DX9GF::GraphicsDevice::GraphicsDevice(IDirect3DDevice9* d3ddev, IDirect3DSurface9* backbuffer) : d3ddev(d3ddev), backbuffer(backbuffer)
{
	d3ddev->GetViewport(&viewport);
}

DX9GF::GraphicsDevice::~GraphicsDevice() {
	if (d3ddev != NULL) d3ddev->Release();
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

HRESULT DX9GF::GraphicsDevice::Clear()
{
	return d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
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

void DX9GF::GraphicsDevice::DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color)
{
	Vertex vertices[] = {
		{ .x=x1, .y=y1, .z=0.0f, .rhw=1.0f, .color=color },
		{ .x=x2, .y=y2, .z=0.0f, .rhw=1.0f, .color=color }
	};
	d3ddev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3ddev->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertices, sizeof(Vertex));
}
