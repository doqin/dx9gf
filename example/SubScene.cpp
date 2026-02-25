#include "SubScene.h"
#include "DX9GFSceneManager.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

void SubScene::Init()
{
}

void SubScene::Update(unsigned long long deltaTime)
{
	if (KEY_DOWN('F')) game->GetSceneManager()->PopScene();
}

void SubScene::Draw(unsigned long long deltaTime)
{
}
