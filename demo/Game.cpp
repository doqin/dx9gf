#include "pch.h"
#include "Game.h"
#include "DebugScene.h"
#include "World1Scene.h"
#include "MainMenu.h"
#include "SettingsScene.h"
void Demo::Game::Init()
{
	IGame::Init();
	auto app = DX9GF::Application::GetInstance();
	DX9GF::Font::AddFont(L"arcade-among-2-r46pv.ttf");
	DX9GF::Font::AddFont(L"statusplz.ttf");
#ifdef TESTING
	this->sceneManager->PushScene(new DebugScene(this, app->GetScreenWidth(), app->GetScreenHeight()));
#else
	// this->sceneManager->PushScene(new World1Scene(this, app->GetScreenWidth(), app->GetScreenHeight()));
	this->sceneManager->PushScene(new MainMenu(this, app->GetScreenWidth(), app->GetScreenHeight()));

#endif
	this->sceneManager->GoToNext();
}
