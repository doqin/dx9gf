#include "pch.h"
#include "DebugScene.h"

void Demo::DebugScene::Init()
{
	draggableManager = std::make_shared<DraggableManager>();
	transformManager = std::make_shared<DX9GF::TransformManager>();
	draggables.push_back(std::make_shared<IDraggable>(transformManager, 50, 70));
	draggables.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	draggables.push_back(std::make_shared<IDraggable>(transformManager, 75, 90, 90, 100));
	draggables.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	draggables.push_back(std::make_shared<IDraggable>(transformManager, 25, 60, -90, -100));
	draggables.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);
	transformManager->RebuildHierarchy();
}

void Demo::DebugScene::Update(unsigned long long deltaTime)
{
	auto inpMan = DX9GF::InputManager::GetInstance();
	inpMan->ReadMouse(deltaTime);
	inpMan->ReadKeyboard(deltaTime);
	if (inpMan->MousePress(DX9GF::InputManager::MouseButton::Middle)) {
		auto dX = inpMan->GetRelativeMouseX();
		auto dY = inpMan->GetRelativeMouseY();
		auto camPos = camera.GetPosition();
		camera.SetPosition(camPos.x - dX, camPos.y - dY);
	}
	auto dZ = inpMan->GetMouseScroll();
	auto camZoom = camera.GetZoom();
	camera.SetZoom(camZoom + dZ / static_cast<float>(1000));
	draggableManager->Update(deltaTime);
	transformManager->UpdateAll();
	camera.Update();
}

void Demo::DebugScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear();
	if (SUCCEEDED(gd->BeginDraw())) {
		draggableManager->Draw(deltaTime);
		gd->EndDraw();
	}
	gd->Present();
}
