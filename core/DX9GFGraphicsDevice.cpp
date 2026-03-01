#include "DX9GFGraphicsDevice.h"
#include <vector>
#include <d3dx9math.h>

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

void DX9GF::GraphicsDevice::DrawRectangle(float x, float y, float width, float height, D3DCOLOR color, bool isFilled)
{
	std::vector<Vertex> vertices = {
		{.x = x, .y = y, .z = 0.0f, .rhw = 1.0f, .color = color },
		{.x = x + width, .y = y, .z = 0.0f, .rhw = 1.0f, .color = color },
		{.x = x + width, .y = y + height, .z = 0.0f, .rhw = 1.0f, .color = color },
		{.x = x, .y = y + height, .z = 0.0f, .rhw = 1.0f, .color = color }
	};
	if (!isFilled) {
		vertices.push_back(vertices[0]);
	}
	d3ddev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	D3DPRIMITIVETYPE primitiveType = isFilled ? D3DPT_TRIANGLEFAN : D3DPT_LINESTRIP;
	d3ddev->DrawPrimitiveUP(primitiveType, isFilled ? 2 : 4, vertices.data(), sizeof(Vertex));
}

void DX9GF::GraphicsDevice::DrawCircle(float centerX, float centerY, float radius, D3DCOLOR color, bool isFilled)
{
	const int SAMPLES = 36; // Number of segments to approximate the circle
	std::vector<Vertex> vertices;

	if (isFilled) vertices.push_back({ .x = centerX, .y = centerY, .z = 0.0f, .rhw = 1.0f, .color = color }); // Center vertex for triangle fan
	for (int i = 0; i <= SAMPLES; ++i) {
		float angle = (2.0f * D3DX_PI * i) / SAMPLES;
		vertices.push_back({
			.x = centerX + radius * cosf(angle),
			.y = centerY + radius * sinf(angle),
			.z = 0.0f,
			.rhw = 1.0f,
			.color = color
		});
	}
	d3ddev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	D3DPRIMITIVETYPE primitiveType = isFilled ? D3DPT_TRIANGLEFAN : D3DPT_LINESTRIP;
	d3ddev->DrawPrimitiveUP(primitiveType, SAMPLES, vertices.data(), sizeof(Vertex));
}
