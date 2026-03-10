#include "DX9GFIGame.h"
#include "DX9GFSceneManager.h"
#include "DX9GFGraphicsDevice.h"
#include <stdexcept>
#include <DxErr.h>

DX9GF::IGame::~IGame()
{
}

HWND DX9GF::IGame::GetHwnd() const
{
	return hwnd;
}

DX9GF::GraphicsDevice* DX9GF::IGame::GetGraphicsDevice()
{
	return graphicsDevice;
}

DX9GF::SceneManager* DX9GF::IGame::GetSceneManager()
{
	return sceneManager;
}

void DX9GF::IGame::Update(unsigned long long deltaTime)
{
	sceneManager->Update(deltaTime);
}

void DX9GF::IGame::Draw(unsigned long long deltaTime)
{
	sceneManager->Draw(deltaTime);
}

void DX9GF::IGame::Init()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL) throw std::runtime_error("Error initializing Direct3D");

	ZeroMemory(&d3dpp, sizeof(d3dpp)); // Xóa mọi thứ về 0 trước khi sử dụng

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.hDeviceWindow = hwnd;

	graphicsDevice = new GraphicsDevice();
	sceneManager = new SceneManager();
	// Create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT, // Dùng card màn hình mặc định
		D3DDEVTYPE_HAL, // Vẽ bằng phần cứng (bằng card màn hình thay vì giả lập)
		hwnd, // Cửa sổ ứng dụng
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, // Các tham số thể hiện của thiết bị
		&graphicsDevice->GetDevice() // đối tượng dev được tạo ra
	);

	if (graphicsDevice->GetDevice() == NULL) throw std::runtime_error("Error creating Direct3D device");

	// create pointer to the back buffer
	graphicsDevice->GetDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &graphicsDevice->GetBackBuffer());
}

void DX9GF::IGame::OnResize(UINT width, UINT height)
{
	if (graphicsDevice == nullptr || graphicsDevice->GetDevice() == nullptr) return;
	if (width == 0 || height == 0) return;

	if (graphicsDevice->GetBackBuffer() != nullptr) {
		graphicsDevice->GetBackBuffer()->Release();
		graphicsDevice->GetBackBuffer() = nullptr;
	}

	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;

	if (auto result = graphicsDevice->GetDevice()->Reset(&d3dpp); SUCCEEDED(result)) {
		graphicsDevice->GetDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &graphicsDevice->GetBackBuffer());
		graphicsDevice->SetViewport(0, 0, width, height, 0.0f, 1.0f);
	}
	else {
		auto error = DXGetErrorDescription(result);
		std::string what = std::string(error, error + wcslen(error));
		throw std::runtime_error("Failed to reset graphics device: " + what);
	}
	sceneManager->OnResize(width, height);
}

void DX9GF::IGame::Dispose()
{
	if (d3d != nullptr) d3d->Release();
	if (graphicsDevice != nullptr) delete graphicsDevice;
	if (sceneManager != nullptr) delete sceneManager;
}