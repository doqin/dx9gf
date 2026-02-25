#include "ExampleGame.h"
#include <format>
#include "DX9GFUtils.h"
#include <string> // Add this at the top if not already present

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

void ExampleGame::Init()
{
	IGame::Init();
	// Creating colorRec
	colorRec = new DX9GF::StaticSprite(&this->graphicsDevice);
	colorRec->CreatePlainTexture(0xFFFFFFFF, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Creating textureRec
	textureRec = new DX9GF::StaticSprite(&this->graphicsDevice);
	textureRec->LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	// Creating mario
	mario = new DX9GF::AnimatedSprite(&this->graphicsDevice);
	mario->LoadSpriteSheet(
		L".\\Resources\\spritesheet.png", 
		DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3),
		156,
		7497
	);
}

void ExampleGame::Update(ULONGLONG deltaTime)
{
	if (KEY_DOWN(VK_ESCAPE)) PostMessage(GetHwnd(), WM_DESTROY, 0, 0);
	float x = 0;
	float y = 0;
	const float velocity = 100;
	if (KEY_DOWN('A')) x -= 1;
	if (KEY_DOWN('D')) x += 1;
	if (KEY_DOWN('W')) y -= 1;
	if (KEY_DOWN('S')) y += 1;
	mario->Translate(x * velocity * deltaTime / 1000, y * velocity * deltaTime / 1000);
}

void ExampleGame::Draw(ULONGLONG deltaTime)
{
	graphicsDevice.Clear();

	if (graphicsDevice.BeginDraw()) {
		//int r, g, b;
		//r = rand() % 255;
		//g = rand() % 255;
		//b = rand() % 255;
		//RECT rect;
		//rect.left = rand() % SCREEN_WIDTH / 2;
		//rect.right = rect.left + rand() % SCREEN_WIDTH / 2;
		//rect.top = rand() % SCREEN_HEIGHT;
		//rect.bottom = rect.top + rand() % SCREEN_HEIGHT / 2;
		//colorRec.SetSrcRect(rect);
		//colorRec.SetPosition(rect.left, rect.top);
		//colorRec.SetColor(D3DCOLOR_XRGB(r,g,b));
		//colorRec.Draw();

		textureRec->Draw();
		mario->Draw();
		graphicsDevice.EndDraw();
	}

	graphicsDevice.Present();
}

void ExampleGame::Dispose()
{
	delete colorRec;
	delete textureRec;
	delete mario;
}
