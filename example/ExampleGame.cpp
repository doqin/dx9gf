#include "ExampleGame.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HRESULT ExampleGame::Init()
{
	return IGame::Init();
}

void ExampleGame::Update()
{
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(hwnd, WM_DESTROY, 0, 0);
}

void ExampleGame::Draw()
{
	
}
