#include "ExampleGame.h"
#include <format>
#include "DX9GFUtils.h"
#include <string> // Add this at the top if not already present

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

std::expected<void, std::wstring> ExampleGame::Init()
{
	auto init_result = IGame::Init();
	// Creating colorRec
	if (!init_result.has_value()) return std::unexpected(init_result.error());

	auto ss_result = DX9GF::StaticSprite::New(&this->graphicsDevice);
	if (ss_result.has_value()) {
		colorRec = std::move(ss_result.value());
	}
	else {
		return std::unexpected(ss_result.error());
	}
	auto cpt_result = colorRec.CreatePlainTexture(0xFFFFFFFF, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!cpt_result.has_value()) {
		return std::unexpected(cpt_result.error());
	}
	// Creating textureRec
	ss_result = DX9GF::StaticSprite::New(&this->graphicsDevice);
	if (ss_result.has_value()) {
		textureRec = std::move(ss_result.value());
	}
	else {
		return std::unexpected(ss_result.error());
	}
	auto lt_result = textureRec.LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	if (!lt_result.has_value()) {
		return std::unexpected(lt_result.error());
	}
	// Creating mario
	auto as_result = DX9GF::AnimatedSprite::New(&this->graphicsDevice);
	if (as_result.has_value()) {
		mario = std::move(as_result.value());
	}
	else {
		return std::unexpected(as_result.error());
	}
	auto lss_result = mario.LoadSpriteSheet(
		L".\\Resources\\spritesheet.png", 
		DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3),
		156,
		7497
	);
	if (!lss_result.has_value()) {
		return std::unexpected(lss_result.error());
	}
	return {};
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
	mario.Translate(x * velocity * deltaTime / 1000, y * velocity * deltaTime / 1000);
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

		textureRec.Draw();
		auto res = mario.Draw();
		if (!res.has_value()) {
			MessageBox(GetHwnd(), res.error().c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION);
		}
		graphicsDevice.EndDraw();
	}

	graphicsDevice.Present();
}
