#include "ExampleGame.h"
#include <format>

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HRESULT ExampleGame::Init()
{
	HRESULT result = IGame::Init();
	if (result != S_OK) return E_FAIL;

	auto ss_result = DX9GF::StaticSprite::New(&this->graphicsDevice);
	if (ss_result.has_value()) {
		colorRec = std::move(ss_result.value());
	}
	else {
		MessageBox(this->GetHwnd(), ss_result.error().c_str(), L"An error has occurred", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	result = colorRec.CreatePlainTexture(0xFFFFFFFF, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (result != D3D_OK) {
		MessageBox(this->GetHwnd(), L"Error trying to create plain texture for colorRec", L"An error has occurred", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	
	ss_result = DX9GF::StaticSprite::New(&this->graphicsDevice);
	if (ss_result.has_value()) {
		textureRec = std::move(ss_result.value());
	}
	else {
		MessageBox(this->GetHwnd(), ss_result.error().c_str(), L"An error has occurred", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	result = textureRec.LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	if (result != D3D_OK) {
		MessageBox(this->GetHwnd(), L"Error trying to load dawg.jpg", L"An error has occurred", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	return S_OK;
}

void ExampleGame::Update()
{
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(GetHwnd(), WM_DESTROY, 0, 0);
	float x = 0;
	float y = 0;
	const float velocity = 3;
	if (KEY_DOWN('A')) x -= 1;
	if (KEY_DOWN('D')) x += 1;
	if (KEY_DOWN('W')) y -= 1;
	if (KEY_DOWN('S')) y += 1;
	textureRec.Translate(x * velocity, y * velocity);
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
		colorRec.SetSrcRect(rect);
		colorRec.SetPosition(rect.left, rect.top);
		colorRec.SetColor(D3DCOLOR_XRGB(r,g,b));
		colorRec.Draw();

		textureRec.Draw();
		graphicsDevice.EndDraw();
	}

	graphicsDevice.Present();
}
