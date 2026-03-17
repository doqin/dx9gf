#include "pch.h"
#include "Game.h"
#include "DebugScene.h"

void Demo::Game::Init()
{
	IGame::Init();
	auto app = DX9GF::Application::GetInstance();
	this->sceneManager->PushScene(new DebugScene(this, app->GetScreenWidth(), app->GetScreenHeight()));
	this->sceneManager->GoToNext();
}
