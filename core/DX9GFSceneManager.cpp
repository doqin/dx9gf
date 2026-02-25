#include "DX9GFSceneManager.h"
#include "DX9GFIScene.h"

DX9GF::SceneManager::~SceneManager()
{
	Dispose();
}

void DX9GF::SceneManager::ChangeScene(IScene* scene)
{
	if (!scenes.empty()) {
		PopScene();
	}
	scenes.push(scene);
	scene->Init();
}

void DX9GF::SceneManager::PushScene(IScene* scene)
{
	scenes.push(scene);
	scene->Init();
}

void DX9GF::SceneManager::PopScene()
{
	auto scene = scenes.top();
	delete scene;
	scenes.pop();
}

void DX9GF::SceneManager::Update(unsigned long long deltaTime)
{
	if (scenes.empty()) {
		throw std::runtime_error("No scene to update!");
	}
	scenes.top()->Update(deltaTime);
}

void DX9GF::SceneManager::Draw(unsigned long long deltaTime)
{
	if (scenes.empty()) {
		throw std::runtime_error("No scene to draw!");
	}
	scenes.top()->Draw(deltaTime);
}

void DX9GF::SceneManager::Dispose()
{
	while (!scenes.empty()) {
		PopScene();
	}
}
