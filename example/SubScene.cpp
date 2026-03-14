#include "pch.h"
#include "SubScene.h"
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <utility>

void SubScene::Init()
{
	inputManager = DX9GF::InputManager::GetInstance();
	auto app = DX9GF::Application::GetInstance();
	auto graphicsDevice = game->GetGraphicsDevice();
	transformManager = std::make_shared<DX9GF::TransformManager>();

	//Kh?i t?o ColliderManager
	colliderManager = std::make_shared<DX9GF::ColliderManager>();

	rects.push_back(std::make_shared<GO::Rectangle>(transformManager, 100, 100, 200, 200));
	rects.push_back(std::make_shared<GO::Rectangle>(transformManager, 50, 200, -200, 200));
	for (auto& rect : rects) {
		//Truy?n colliderManager thay vì &worldColliders
		rect->Init(game->GetGraphicsDevice(), &camera, colliderManager);
	}

	ellipses.push_back(std::make_shared<GO::Ellipse>(transformManager, 100, 100, 0, 0));
	ellipses.push_back(std::make_shared<GO::Ellipse>(transformManager, 200, 50, 0, -200));
	for (auto& ellipse : ellipses) {
		//Truy?n colliderManager thay vì &worldColliders
		ellipse->Init(game->GetGraphicsDevice(), &camera, colliderManager);
	}

	transformManager->RebuildHierarchy();
}

void SubScene::Update(unsigned long long deltaTime)
{
	inputManager->ReadKeyboard(deltaTime);
	inputManager->ReadMouse(deltaTime);
	if (inputManager->KeyDown(DIK_F)) {
		game->GetSceneManager()->GoToPrevious();
		return; // return otherwise we get a use after free situation
	}
	if (inputManager->KeyPress(DIK_LCONTROL) && inputManager->KeyDown(DIK_S)) {
		game->GetSaveManager()->Save("./Save/temp.sav");
	}
	tf::Executor executor;
	tf::Taskflow taskflow;
	taskflow.for_each(rects.begin(), rects.end(), [deltaTime](std::shared_ptr<GO::Rectangle> rect) {
		rect->Update(deltaTime);
	});
	taskflow.for_each(ellipses.begin(), ellipses.end(), [deltaTime](std::shared_ptr<GO::Ellipse> ellipse) {
		ellipse->Update(deltaTime);
	});
	executor.run(taskflow).wait();
	transformManager->UpdateAll();
	camera.Update();
}

void SubScene::Draw(unsigned long long deltaTime)
{
	auto dev = game->GetGraphicsDevice();
	dev->Clear();
	if (SUCCEEDED(dev->BeginDraw())) {
		auto app = DX9GF::Application::GetInstance();
		auto width = app->GetScreenWidth();
		auto height = app->GetScreenHeight();
		DX9GF::Debug::DrawGrid(
			dev,
			0,
			0,
			width,
			height,
			32,
			32,
			0xFFFF0000);
		for (auto& rect : rects) {
			rect->Draw(deltaTime);
		}
		for (auto& ellipse : ellipses) {
			ellipse->Draw(deltaTime);
		}
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}

std::string SubScene::GetSaveID() const
{
	return "SubScene";
}

void SubScene::GenerateSaveData(nlohmann::json& outData)
{
}

void SubScene::RestoreSaveData(const nlohmann::json& inData)
{
}
