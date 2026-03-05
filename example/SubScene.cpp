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
	rects.push_back(std::make_shared<GO::Rectangle>(transformManager, 100, 100, 200, 200));
	rects.push_back(std::make_shared<GO::Rectangle>(transformManager, 50, 200, -200, 200));
	for (auto& rect : rects) {
		rect->Init(game->GetGraphicsDevice(), &camera, &worldColliders);
	}
	circle = std::make_shared<GO::Ellipse>(transformManager, 100, 100, 0, 0);
	circle->Init(game->GetGraphicsDevice(), &camera);
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
	auto& js = game->GetJobSystem();
	js.DispatchBatch({
		.function = [deltaTime](void* batch, size_t idx) {
			static_cast<std::shared_ptr<GO::Rectangle>*>(batch)[idx]->Update(deltaTime);
		},
		.batch=rects.data(),
		.startIdx = 0,
		.endIdx = 2 // for some reason this is +1 than intended :P
	}, 1);
	js.Dispatch({
		.function = [deltaTime](void* data) {
			static_cast<GO::Ellipse*>(data)->Update(deltaTime);
		},
		.data = static_cast<void*>(circle.get())
	});
	js.Wait();
	transformManager->UpdateAll(js);
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
		circle->Draw(deltaTime);
		dev->EndDraw();
	}
	dev->Present();
}

void SubScene::Dispose()
{
}
