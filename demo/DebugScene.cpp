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
	containers.push_back(std::make_shared<IContainer>(transformManager, 100, 30, -100, -100));
	containers.back()->Init(draggableManager, game->GetGraphicsDevice(), &camera);

	//just load resource (texture, font) 1 time and reuse it all over again
	uiSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
	uiSheetTex->LoadTexture(L"Resources/ui-pack.png");	
	//Everything works fine with .png, but .bmp is causing coordinate issues. Idk how to fix bruh
	myFont = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arial", 24);

	//create 1 text button and 2 icon buttons
	auto btnTextExit = std::make_shared<DX9GF::cTextButton>(
		transformManager,
		0, 0, 150, 50, //x,y,w,h
		"EXIT GAME", //display text
		myFont.get(), //reuse myFont
		[](DX9GF::ITrigger* t)
		{
			PostQuitMessage(0);
		}
	);
	btnTextExit->Init(&camera);
	uiButtons.push_back(btnTextExit);

	//test textbutton setter
	//btnTextExit->SetBackgroundColors(D3DXCOLOR(0.8f, 0, 0, 1), D3DXCOLOR(1, 0, 0, 1), D3DXCOLOR(0, 1, 0, 1))
	//         ->SetTextColors(0xFFFFA500, 0xFF000000, 0xFFFFFFFF);

	auto quitBtn = std::make_shared<DX9GF::cIconButton>(
		transformManager,
		200, 200, 62, 30, //x,y,w,h display
		uiSheetTex,
		385, 449, 62, 30, 2, //startX, startY, w, h, spacing
		[](DX9GF::ITrigger* t)
		{
			PostQuitMessage(0); //set button's logic here
		}
	);
	quitBtn->Init(&camera);
	uiButtons.push_back(quitBtn);

	auto continueBtn = std::make_shared<DX9GF::cIconButton>(
		transformManager,
		150, 150, 94, 30,
		uiSheetTex,
		577, 193, 94, 30, 34,
		[](DX9GF::ITrigger* t)
		{
			//switch scene(?)
		}
	);
	continueBtn->Init(&camera);
	uiButtons.push_back(continueBtn);

	//test function
	//continueBtn->ChangeSpriteCoords(500, 2, 60, 32, 20);

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

	//update all UI button
	for (auto& btn : uiButtons) {
		btn->Update(deltaTime);
	}
	camera.Update();
}

void Demo::DebugScene::Draw(unsigned long long deltaTime)
{
	auto gd = game->GetGraphicsDevice();
	gd->Clear();
	if (SUCCEEDED(gd->BeginDraw())) {
		draggableManager->Draw(deltaTime);

		//draw all UI button
		for (auto& btn : uiButtons)
		{
			btn->Draw(&this->camera, gd, deltaTime);
		}

		gd->EndDraw();
	}
	gd->Present();
}
