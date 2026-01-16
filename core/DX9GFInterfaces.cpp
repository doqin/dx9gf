#include "DX9GFInterfaces.h"

DX9GF::IGame::~IGame()
{
	this->Dispose();
}

HRESULT DX9GF::IGame::Init()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL) {
		MessageBox(hwnd, L"Error initializing Direct3D", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // Xóa mọi thứ về 0 trước khi sử dụng

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.hDeviceWindow = hwnd;

	// Create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT, // Dùng card màn hình mặc định
		D3DDEVTYPE_HAL, // Vẽ bằng phần cứng (bằng card màn hình thay vì giả lập)
		hwnd, // Cửa sổ ứng dụng
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, // Các tham số thể hiện của thiết bị
		&d3ddev // đối tượng dev được tạo ra
	);

	if (d3ddev == NULL) {
		MessageBox(hwnd, L"Error creating Direct3D device", L"Error", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	// clear the backbuffer to black
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// create pointer to the back buffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	return 1;
}

void DX9GF::IGame::Dispose()
{
	if (d3ddev != NULL) d3ddev->Release();

	if (d3d != NULL) d3d->Release();
}
