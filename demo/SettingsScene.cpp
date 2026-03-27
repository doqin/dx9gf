#include "pch.h"
#include "SettingsScene.h"
#include "resource.h"
#include "IconButton.h"
#include "SettingsManager.h"
namespace Demo
{
	void SettingsScene::DrawString(std::wstring text, float x, float y, D3DCOLOR color)
	{
		if (!font) return;

		// Chuyển đổi từ tọa độ tâm (World) sang tọa độ màn hình (Screen)
		// Giả sử camera của bạn đang ở chính giữa
		int screenX = (int)(lastScreenWidth / 2.0f + x);
		int screenY = (int)(lastScreenHeight / 2.0f + y);

		RECT rect;
		// Đặt vùng vẽ đủ rộng sang bên phải
		SetRect(&rect, screenX, screenY, screenX + 300, screenY + 50);

		font->GetRawFont()->DrawTextW(
			NULL,
			text.c_str(),
			-1,
			&rect,
			DT_LEFT | DT_NOCLIP, // Canh lề trái từ điểm (x, y)
			color
		);

	}

	void SettingsScene::ResetListening()
	{
		isListeningUp = isListeningDown = isListeningLeft = isListeningRight = false;

		// Đưa các nút về trạng thái bình thường
		if (btnUp)
			btnUp->SetState(Demo::IButton::ButtonState::IDLE);
		// if (btnDown) btnDown->SetState(Demo::IButton::ButtonState::IDLE); ...		
	}

	std::wstring ToWString(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}

	std::string GetKeyName(int vkCode) {
		if (vkCode <= 0) return "None";

		char name[64];
		// Chuyển Virtual Key sang Scan Code
		UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

		// Tạo tham số lParam cho GetKeyNameTextA
		// bit 16-23 là scan code
		LONG lParam = scanCode << 16;

		// Xử lý các phím mở rộng (như phím mũi tên, Insert, Delete,...)
		if (vkCode >= 33 && vkCode <= 46 || vkCode >= 91 && vkCode <= 93) {
			lParam |= 0x01000000; // Set bit 24 (Extended key flag)
		}

		if (GetKeyNameTextA(lParam, name, 64)) {
			return std::string(name);
		}

		return "Key " + std::to_string(vkCode);
	}

	void SettingsScene::UpdateLayout(int screenW, int screenH)
	{
		//BACKGROUND - use aspect fill
		float bgImageW = (float)bgTex->GetWidth();
		float bgImageH = (float)bgTex->GetHeight();

		if (!bgSprite)
		{
			bgSprite = std::make_shared<DX9GF::StaticSprite>(bgTex.get());
			bgSprite->SetSrcRect({ 0, 0, (LONG)bgImageW, (LONG)bgImageH });
		}

		float bgScaleX = screenW / bgImageW;
		float bgScaleY = screenH / bgImageH;
		float bgFinalScale = std::max(bgScaleX, bgScaleY);

		bgSprite->SetScale(bgFinalScale);
		bgSprite->SetOrigin(bgImageW / 2.0f, bgImageH / 2.0f);
		bgSprite->SetPosition(0, 0);

		//TITLE - use scale to fit (have a limit)
		float titleImageW = (float)titleTex->GetWidth();
		float titleImageH = (float)titleTex->GetHeight();

		if (!titleSprite) {
			titleSprite = std::make_shared<DX9GF::StaticSprite>(titleTex.get());
			titleSprite->SetSrcRect({ 0, 0, (LONG)titleImageW, (LONG)titleImageH });
		}

		float maxTitleWidth = screenW * 0.5f;
		float titleScale = maxTitleWidth / titleImageW;
		float maxTitleHeight = screenH * 0.4f;

		if ((titleImageH * titleScale) > maxTitleHeight) {
			titleScale = maxTitleHeight / titleImageH;
		}

		titleSprite->SetScale(titleScale);
		titleSprite->SetOrigin(titleImageW / 2.0f, titleImageH / 2.0f);
		titleSprite->SetPosition(0, -screenH * 0.20f);

		//spacing between buttons
		float spacingY = 10.0f;
		//start drawing from 40% of the screen height
		float startY = -screenH * 0.10f;

		std::shared_ptr<Demo::IButton> buttons[] = { backButton,btnUp };
		float currentY = startY;

		for (auto& btn : buttons)
		{
			if (btn)
			{
				//center X
				float posX = -(btn->GetWidth() / 2.0f);
				btn->SetLocalPosition(posX, currentY);

				//button spacing
				currentY += btn->GetHeight() + spacingY;
			}
		}

		// Cấu hình vị trí cột label
		float columnLeftX = -screenW * 0.25f; // Nằm bên trái tâm màn hình 25% chiều rộng
		float labelX = -screenW * 0.25f;  // Vị trí cột chữ
		float buttonX = screenW * 0.05f; // Vị trí cột nút (nằm bên phải tâm một chút)
		float spacing = 30.0f;
		// Lưu các vị trí này vào biến thành viên nếu cần dùng cho Slider/Button sau này
	// Ví dụ: 
		//labelMasterPos = { columnLeftX, startY };
		//labelMusicPos = { columnLeftX, startY + rowSpacing };

		if (btnUp)    btnUp->SetLocalPosition(buttonX, startY + spacing * 3.5f);
	}

	void SettingsScene::Init()
	{
		transformManager = std::make_shared<DX9GF::TransformManager>();

		auto app = DX9GF::Application::GetInstance();
		lastScreenWidth = app->GetScreenWidth();
		lastScreenHeight = app->GetScreenHeight();

		//add custom font
		DX9GF::Font::AddFont(L"arcade-among-2-r46pv.ttf");
		font = std::make_shared<DX9GF::Font>(
			game->GetGraphicsDevice(),
			L"Arcade Among 2 R46PV",
			24,
			0,
			FW_BOLD
		);
		//load textures
		buttonSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		buttonSheetTex->LoadTexture(L"ui-pack.png");

		bgTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		bgTex->LoadTexture(IDB_PNG2);

		titleTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		titleTex->LoadTexture(IDB_PNG3);

		//buttons init
		backButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 94, 30, buttonSheetTex, 3);
		backButton->SetSpriteCoords(1, 417, 62, 30, 2);
		backButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) 
			{ 
				this->isGoingBack = true;
			});

		btnUp = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 100, 40, buttonSheetTex, 2);
		btnUp->SetSpriteCoords(1, 481, 30, 30, 2);
		btnUp->SetOnReleaseLeft([this](DX9GF::ITrigger* t) 
			{ 
				this->ResetListening(); // Tắt các nút khác đang chờ (nếu có)
				this->isListeningUp = true;
				// Ép nút vào trạng thái LISTENING
				btnUp->SetState(Demo::IButton::ButtonState::LISTENING);
			});

		//active buttons
		std::shared_ptr<Demo::IButton> buttons[] = { backButton, btnUp };
		for (auto& btn : buttons)
		{
			if (btn)
			{
				btn->Init(&camera);
				uiButtons.push_back(btn);
			}
		}

		//call it to setup the update layout
		UpdateLayout(lastScreenWidth, lastScreenHeight);

		transformManager->RebuildHierarchy();
	}

	void SettingsScene::Update(unsigned long long deltaTime)
	{
		auto inpMan = DX9GF::InputManager::GetInstance();
		inpMan->ReadMouse(deltaTime);
		inpMan->ReadKeyboard(deltaTime);

		auto app = DX9GF::Application::GetInstance();
		int currentWidth = app->GetScreenWidth();
		int currentHeight = app->GetScreenHeight();

		//check the size and update if there is a change
		if (currentWidth != lastScreenWidth || currentHeight != lastScreenHeight)
		{
			UpdateLayout(currentWidth, currentHeight);
			lastScreenWidth = currentWidth;
			lastScreenHeight = currentHeight;
		}

		for (auto& button : uiButtons)
		{
			button->Update(deltaTime);
		}

		transformManager->UpdateAll();
		camera.Update();

		// 2. LOGIC BẮT PHÍM (Đảm bảo logic này chạy ổn định)
		if (isListeningUp) // Làm tương tự cho Down, Left, Right
		{
			for (int i = 0x07; i <= 0xFE; i++)
			{
				if (GetAsyncKeyState(i) & 0x8000)
				{
					// BỎ QUA NẾU LÀ CHUỘT (VK_LBUTTON = 1, VK_RBUTTON = 2, ...)
					if (i >= 1 && i <= 6) continue;

					// A. Lưu phím vào Manager
					SettingsManager::GetInstance()->SetKeybind("MoveUp", i);

					// B. TRẢ NÚT VỀ IDLE (Mở khóa)
					btnUp->SetState(IButton::ButtonState::IDLE);

					// C. Tắt cờ Scene
					isListeningUp = false;

					SettingsManager::GetInstance()->SaveSettings();
					break;
				}
			}
		}
		if (this->isGoingBack)
		{
			auto sm = this->game->GetSceneManager();
			sm->GoToPrevious();
			sm->PopScene();
			return;
		}
	}

	void SettingsScene::Draw(unsigned long long deltaTime)
	{
		auto gd = game->GetGraphicsDevice();
		gd->Clear();
		if (SUCCEEDED(gd->BeginDraw())) {
			if (bgSprite)
			{
				bgSprite->Begin();
				bgSprite->Draw(camera, deltaTime);
				bgSprite->End();
			}

			if (titleSprite)
			{
				titleSprite->Begin();
				titleSprite->Draw(camera, deltaTime);
				titleSprite->End();
			}
			// --- VẼ LABEL TĨNH ---
			float labelX = -lastScreenWidth * 0.25f;
			float startY = -lastScreenHeight * 0.02f;
			float spacing = 30.0f;

			DrawString(L"MASTER VOLUME", labelX, startY, D3DCOLOR_XRGB(255, 255, 255));
			DrawString(L"MUSIC VOLUME", labelX, startY + spacing, D3DCOLOR_XRGB(255, 255, 255));
			DrawString(L"SFX VOLUME", labelX, startY + spacing * 2, D3DCOLOR_XRGB(255, 255, 255));

			DrawString(L"MOVE UP", labelX, startY + spacing * 3.5f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE DOWN", labelX, startY + spacing * 4.5f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE LEFT", labelX, startY + spacing * 5.5f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE RIGHT", labelX, startY + spacing * 6.5f, D3DCOLOR_XRGB(200, 200, 255));


			for (auto& btn : uiButtons)
			{
				btn->Draw(&this->camera, gd, deltaTime);
			}

			// --- VẼ TEXT ĐÈ LÊN NÚT KEYBIND ---
			auto sm = SettingsManager::GetInstance();

			// Lambda hỗ trợ vẽ text căn giữa vào Button
			auto DrawKeyOnButton = [&](std::string action, std::shared_ptr<IconButton> btn, bool listening) {
				if (!btn) return;

				std::wstring text = listening ? L"???" : ToWString(GetKeyName(sm->GetKeybind(action)));
				D3DCOLOR color = listening ? D3DCOLOR_XRGB(255, 255, 0) : D3DCOLOR_XRGB(255, 255, 255);

				// Căn giữa tương đối:
				// Lấy vị trí Local của nút + offset để vào giữa lòng nút
				float textX = btn->GetLocalX() + 10; // Tùy chỉnh số này để khớp box của bạn
				float textY = btn->GetLocalY() + (btn->GetHeight() / 4.0f);

				DrawString(text, textX, textY, color);
				};

			DrawKeyOnButton("MoveUp", btnUp, isListeningUp);

			gd->EndDraw();
		}
		gd->Present();
	}

}
