#include "pch.h"
#include "MainScene.h"
#include "SubScene.h"
#include "DX9GFSceneManager.h"

void ExampleGame::Init()
{
	IGame::Init();
	auto app = DX9GF::Application::GetInstance();
	size_t sceneIdx = 0;
	auto mainScene = new MainScene(this, app->GetScreenWidth(), app->GetScreenHeight());
	auto subScene = new SubScene(this, app->GetScreenWidth(), app->GetScreenHeight());
	sceneMap[mainScene->GetSaveID()] = sceneIdx++;
	sceneMap[subScene->GetSaveID()] = sceneIdx++;
	sceneManager->PushScene(mainScene);
	sceneManager->PushScene(subScene);
	sceneManager->GoToNext();

	saveManager->Register(this);
	saveManager->Load("./Save/temp.sav");
}

std::string ExampleGame::GetSaveID() const
{
	return "Game";
}

void ExampleGame::GenerateSaveData(nlohmann::json& outData)
{
	auto currentScene = dynamic_cast<DX9GF::ISaveable*>(sceneManager->GetCurrentScene());
	if (currentScene) outData["current_scene"] = currentScene->GetSaveID();
	for (size_t i = 0; i < sceneManager->GetSceneCount(); i++) {
		nlohmann::json data;
		auto scene = dynamic_cast<DX9GF::ISaveable*>(sceneManager->GetScene(i));
		if (scene) { 
			scene->GenerateSaveData(data);
			outData["scene_data"][scene->GetSaveID()] = data;
		}
	}
}

void ExampleGame::RestoreSaveData(const nlohmann::json& inData)
{
	if (inData.contains("current_scene")) {
		sceneManager->GoToScene(sceneMap[inData["current_scene"]]);
	}
	if (inData.contains("scene_data")) {
		for (auto& [key, value] : inData["scene_data"].items()) {
			if (sceneMap.contains(key)) {
				auto scene = dynamic_cast<DX9GF::ISaveable*>(sceneManager->GetScene(sceneMap[key]));
				if (scene) scene->RestoreSaveData(value);
			}
		}
	}
}

DX9GF::SaveManager* ExampleGame::GetSaveManager()
{
	return saveManager;
}
