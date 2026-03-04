#include "DX9GFSceneManager.h"
#include "DX9GFIScene.h"
#include <stdexcept>
#include "DX9GFApplication.h"

DX9GF::SceneManager::~SceneManager()
{
	Dispose();
}

void DX9GF::SceneManager::ChangeScene(IScene* scene)
{
	if (!scenes.empty()) {
		PopScene();
	}
	scenes.push_back(scene);
	scene->Init();
}

void DX9GF::SceneManager::PushScene(IScene* scene)
{
	scenes.push_back(scene);
	scene->Init();
}

void DX9GF::SceneManager::PopScene()
{
	auto scene = scenes.back();
	delete scene;
	scenes.pop_back();
}

void DX9GF::SceneManager::Update(unsigned long long deltaTime)
{
	if (scenes.empty()) {
		throw std::runtime_error("No scene to update!");
	}
	scenes.back()->Update(deltaTime);
}

void DX9GF::SceneManager::Draw(unsigned long long deltaTime)
{
	if (scenes.empty()) {
		throw std::runtime_error("No scene to draw!");
	}
	scenes.back()->Draw(deltaTime);
}

void DX9GF::SceneManager::OnResize(int width, int height)
{
	for (auto& scene : scenes) {
		auto [cameraWidth, cameraHeight] = scene->GetCamera().GetScreenResolution();
		auto app = DX9GF::Application::GetInstance();
		auto previousWidth = app->GetScreenWidth();
		auto previousHeight = app->GetScreenHeight();
		scene->GetCamera().SetScreenResolution(cameraWidth * width / previousWidth, cameraHeight * height / previousHeight);
	}
}

void DX9GF::SceneManager::Dispose()
{
	while (!scenes.empty()) {
		PopScene();
	}
}
