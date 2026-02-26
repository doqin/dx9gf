#include "SubScene.h"
#include "DX9GFSceneManager.h"

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->KeySnapShot(deltaTime);
	if (inputManager->KeyDown(DIK_F)) game->GetSceneManager()->PopScene();
}

void SubScene::Draw(unsigned long long deltaTime)
{
}
