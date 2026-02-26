#include "MainScene.h"
#include <DX9GFUtils.h>
#include "DX9GFGraphicsDevice.h"
#include "DX9GFSceneManager.h"
#include "SubScene.h"

void MainScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	// Creating colorRec
	colorRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	colorRec->CreatePlainTexture(0xFFFFFFFF, 500, 500);
	// Creating textureRec
	textureRec = new DX9GF::StaticSprite(game->GetGraphicsDevice());
	textureRec->LoadTexture(L".\\Resources\\dawg.jpg", 100, 100);
	// Creating mario
	mario = new DX9GF::AnimatedSprite(game->GetGraphicsDevice());
	mario->LoadSpriteSheet(
		L".\\Resources\\spritesheet.png",
		DX9GF::Utils::CreateFrames(156, 7497, 39, 51, 3),
		156,
		7497
	);
}

void MainScene::Update(unsigned long long deltaTime)
{
	inputManager->KeySnapShot(deltaTime);
	if (inputManager->KeyDown(DIK_ESCAPE)) PostMessage(game->GetHwnd(), WM_DESTROY, 0, 0);
	float xDir = 0;
	float yDir = 0;
	const float velocity = 200;
	if (inputManager->KeyPress(DIK_A)) xDir -= 1;
	if (inputManager->KeyPress(DIK_D)) xDir += 1;
	if (inputManager->KeyPress(DIK_W)) yDir -= 1;
	if (inputManager->KeyPress(DIK_S)) yDir += 1;
	if (inputManager->KeyDown(DIK_F)) game->GetSceneManager()->PushScene(new SubScene(game));
	mario->Translate(xDir * velocity * deltaTime / 1000, yDir * velocity * deltaTime / 1000);
}

void MainScene::Dispose()
{
	delete colorRec;
	delete textureRec;
	delete mario;
}

void MainScene::Draw(unsigned long long deltaTime)
{
	
	game->GetGraphicsDevice()->Clear();

	if (game->GetGraphicsDevice()->BeginDraw()) {
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
		game->GetGraphicsDevice()->EndDraw();
	}

	game->GetGraphicsDevice()->Present();
}