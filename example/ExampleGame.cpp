#include "ExampleGame.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HRESULT ExampleGame::Init()
{
	HRESULT result = IGame::Init();
	if (result != 1) return 0;

	auto obj = DX9GF::StaticSprite::New(&this->graphicsDevice, 300, 300);
	if (obj.has_value()) {
		obj1 = obj.value();
	}
	else {
		MessageBox(this->GetHwnd(), obj.error().c_str(), L"An error has occurred", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	return 1;
}

void ExampleGame::Update()
{
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(GetHwnd(), WM_DESTROY, 0, 0);
}

void ExampleGame::Draw()
{
	if (graphicsDevice.BeginDraw()) {
		int r, g, b;
		r = rand() % 255;
		g = rand() % 255;
		b = rand() % 255;
		RECT rect;
		rect.left = rand() % SCREEN_WIDTH / 2;
		rect.right = rect.left + rand() % SCREEN_WIDTH / 2;
		rect.top = rand() % SCREEN_HEIGHT;
		rect.bottom = rect.top + rand() % SCREEN_HEIGHT / 2;
		obj1.FillColor(D3DCOLOR_XRGB(r, g, b));
		obj1.SetRect(rect);
		obj1.Draw();
		graphicsDevice.EndDraw();
	}

	graphicsDevice.Update();
}
