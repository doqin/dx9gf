#include "MainScene.h"
#include "DX9GFSceneManager.h"

void ExampleGame::Init()
{
	IGame::Init();
	auto app = DX9GF::Application::GetInstance();
	sceneManager->PushScene(new MainScene(this, app->GetScreenWidth(), app->GetScreenHeight()));
	sceneManager->GoToNext();
}

DX9GF::JobSystem& ExampleGame::GetJobSystem()
{
	return jobSystem;
}