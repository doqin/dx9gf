#include "pch.h"
#include "MainMenu.h"
// Include các class cụ thể kế thừa từ IButton (ví dụ: TextButton, IconButton)
#include "resource.h"
#include "IconButton.h"
namespace Demo
{
	void MainMenu::Init()
	{
		// 1. Khởi tạo TransformManager (nếu hệ thống của bạn yêu cầu)
		transformManager = std::make_shared<DX9GF::TransformManager>();

		// 2. Load Texture Sheet cho toàn bộ UI
		// Giả sử hệ thống của bạn có hàm LoadTexture tĩnh hoặc thông qua Game object
		uiSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		uiSheetTex->LoadTexture(L"ui-pack.png");

		// 3. Khởi tạo Background và Title
		// Truyền ID hoặc đường dẫn texture tùy vào thiết kế constructor của StaticSprite
		backgroundTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		titleTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());

		backgroundTex->LoadTexture(IDB_PNG2);
		titleTex->LoadTexture(IDB_PNG3);

		backgroundSprite = std::make_shared<DX9GF::StaticSprite>(backgroundTex.get());

		RECT bgRect;
		bgRect.left = 0;
		bgRect.top = 0;
		bgRect.right = backgroundTex->GetWidth();  // Thay bằng chiều rộng thực tế của ảnh nền
		bgRect.bottom = backgroundTex->GetHeight(); // Thay bằng chiều cao thực tế của ảnh nền
		backgroundSprite->SetSrcRect(bgRect);
		backgroundSprite->SetScale(1.0f); // Bắt buộc để ảnh không bị thu nhỏ về 0
		backgroundSprite->SetPosition(-400, -400); // Đặt tại góc trái trên

		titleSprite = std::make_shared<DX9GF::StaticSprite>(titleTex.get());


		//RECT titleRect;
		//titleRect.left = 0;
		//titleRect.top = 0;
		//titleRect.right = 400; // Chiều rộng của logo
		//titleRect.bottom = 150; // Chiều cao của logo
		//titleSprite->SetSrcRect(titleRect);
		titleSprite->SetScale(0.5f); // Bắt buộc để ảnh không bị thu nhỏ về 0
		// Đặt title ở vị trí nào đó (ví dụ x=100, y=50)
		titleSprite->SetPosition(-400, -300);

		backgroundSprite->Begin();
		auto quitBtn = std::make_shared<Demo::IconButton
		>(
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

		auto continueBtn = std::make_shared<Demo::IconButton>(
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

	void MainMenu::Update(unsigned long long deltaTime)
	{
		auto inpMan = DX9GF::InputManager::GetInstance();
		inpMan->ReadMouse(deltaTime);
		inpMan->ReadKeyboard(deltaTime);

		for (auto& button : uiButtons)
		{
			button->Update(deltaTime);
		}

		transformManager->UpdateAll();
		camera.Update();
	}

	void MainMenu::Draw(unsigned long long deltaTime)
	{
		auto gd = game->GetGraphicsDevice();
		gd->Clear();
		if (SUCCEEDED(gd->BeginDraw())) {
			// THỨ TỰ VẼ TỪ DƯỚI LÊN TRÊN

			if (backgroundSprite)
			{
				backgroundSprite->Begin(); // BẮT BUỘC PHẢI CÓ
				backgroundSprite->Draw(camera, deltaTime);
				backgroundSprite->End();   // BẮT BUỘC PHẢI CÓ
			}

			// 2. Vẽ Tiêu đề / Logo
			if (titleSprite)
			{
				titleSprite->Begin(); // BẮT BUỘC PHẢI CÓ
				titleSprite->Draw(camera, deltaTime);
				titleSprite->End();   // BẮT BUỘC PHẢI CÓ
			}

			// 3. Vẽ toàn bộ UI Buttons lên trên cùng
			for (auto& btn : uiButtons)
			{
				btn->Draw(&this->camera, gd, deltaTime);
			}
			gd->EndDraw();
		}
		gd->Present();
	}

}
