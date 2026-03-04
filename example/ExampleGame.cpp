#include "MainScene.h"
#include "DX9GFSceneManager.h"

void ExampleGame::Init()
{
	IGame::Init();
	sceneManager->PushScene(new MainScene(this));
}

DX9GF::JobSystem& ExampleGame::GetJobSystem()
{
	return jobSystem;
}