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
	//Load cursor textures
	auto input = DX9GF::InputManager::GetInstance();
	auto gd = GetGraphicsDevice();

	//hotspot (hX, hY) must be multiplied by the scale factor
	input->AddCursor(DX9GF::InputManager::CursorType::CURSOR, gd, L"cursor.png", 0.2f, 0.0f, 0.0f);
	input->AddCursor(DX9GF::InputManager::CursorType::POINTER, gd, L"pointer.png", 0.2f, 0.0f, 0.0f);
	input->AddCursor(DX9GF::InputManager::CursorType::CLICK, gd, L"click.png", 0.2f, 4.0f, 6.0f);
	input->AddCursor(DX9GF::InputManager::CursorType::GRAB, gd, L"grab.png", 0.2f, 14.8f, 14.8);
	input->AddCursor(DX9GF::InputManager::CursorType::TEXTSELECT, gd, L"text-select.png", 0.2f, 10.0f, 16.0f);
#ifdef TESTING
	this->sceneManager->PushScene(new DebugScene(this, app->GetScreenWidth(), app->GetScreenHeight()));
#else
	// this->sceneManager->PushScene(new World1Scene(this, app->GetScreenWidth(), app->GetScreenHeight()));
	this->sceneManager->PushScene(new MainMenu(this, app->GetScreenWidth(), app->GetScreenHeight()));

#endif
	this->sceneManager->GoToNext();
}
