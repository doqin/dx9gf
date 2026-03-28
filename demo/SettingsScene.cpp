#include "pch.h"
#include "SettingsScene.h"
#include "resource.h"
#include "IconButton.h"
#include "SettingsManager.h"
#include <algorithm> // Bổ sung cho std::max, std::min

namespace Demo
{
	// ==========================================
	// HELPER LẤY TÊN PHÍM & CHUỖI
	// ==========================================
	std::wstring ToWString(const std::string& s)
	{
		return std::wstring(s.begin(), s.end());
	}

	std::string GetKeyName(int vkCode)
	{
		if (vkCode <= 0) return "None";
		char name[64];
		UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
		LONG lParam = scanCode << 16;
		if ((vkCode >= 33 && vkCode <= 46) || (vkCode >= 91 && vkCode <= 93)) {
			lParam |= 0x01000000;
		}
		if (GetKeyNameTextA(lParam, name, 64)) return std::string(name);
		return "Key " + std::to_string(vkCode);
	}

	// ==========================================
	// CÁC HÀM VẼ (DRAW)
	// ==========================================
	void SettingsScene::DrawString(std::wstring text, float x, float y, D3DCOLOR color, DWORD format)
	{
		if (!font) return;
		int screenX = (int)(lastScreenWidth / 2.0f + x);
		int screenY = (int)(lastScreenHeight / 2.0f + y);

		RECT rect;
		if (format & DT_RIGHT) SetRect(&rect, screenX - 500, screenY, screenX, screenY + 50);
		else if (format & DT_CENTER) SetRect(&rect, screenX - 100, screenY, screenX + 100, screenY + 50);
		else SetRect(&rect, screenX, screenY, screenX + 300, screenY + 50);

		font->GetRawFont()->DrawTextW(NULL, text.c_str(), -1, &rect, format | DT_NOCLIP, color);
	}

	void SettingsScene::DrawVolumeTrack(std::shared_ptr<DX9GF::StaticSprite> bg, std::shared_ptr<DX9GF::StaticSprite> fill, float vol, RECT originalRect, unsigned long long deltaTime)
	{
		if (bg) { bg->Begin(); bg->Draw(camera, deltaTime); bg->End(); }
		if (fill && bg) {
			RECT r = originalRect;
			float fullWidth = (float)(originalRect.right - originalRect.left);
			r.right = r.left + (LONG)(fullWidth * vol);
			fill->SetSrcRect(r);
			fill->SetScale(SLIDER_SCALE_X, 1.0f);
			fill->Begin(); fill->Draw(camera, deltaTime); fill->End();
		}
	}

	void SettingsScene::DrawKeybindButton(const std::string& action, std::shared_ptr<IconButton> btn, bool listening)
	{
		if (!btn || !font) return;
		auto sm = SettingsManager::GetInstance();
		std::wstring text = listening ? L"???" : ToWString(GetKeyName(sm->GetKeybind(action)));
		D3DCOLOR color = listening ? D3DCOLOR_XRGB(255, 255, 0) : D3DCOLOR_XRGB(255, 255, 255);

		int btnScreenX = (int)(lastScreenWidth / 2.0f + btn->GetLocalX());
		int btnScreenY = (int)(lastScreenHeight / 2.0f + btn->GetLocalY());

		RECT r;
		SetRect(&r, btnScreenX, btnScreenY, btnScreenX + (int)btn->GetWidth(), btnScreenY + (int)btn->GetHeight());

		// Bù trừ visual offset bằng Hằng Số (Không còn hardcode dơ)
		r.top += BTN_VISUAL_SHADOW_OFFSET;
		r.bottom += BTN_VISUAL_SHADOW_OFFSET;

		font->GetRawFont()->DrawTextW(NULL, text.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP, color);
	}

	// ==========================================
	// KHỞI TẠO VÀ CẬP NHẬT (INIT & UPDATE)
	// ==========================================
	void SettingsScene::ResetListening()
	{
		isListeningUp = isListeningDown = isListeningLeft = isListeningRight = false;
		if (btnUp) btnUp->SetState(Demo::IButton::ButtonState::IDLE);
		if (btnDown) btnDown->SetState(Demo::IButton::ButtonState::IDLE);
		if (btnLeft) btnLeft->SetState(Demo::IButton::ButtonState::IDLE);
		if (btnRight) btnRight->SetState(Demo::IButton::ButtonState::IDLE);
	}

	void SettingsScene::UpdateLayout(int screenW, int screenH)
	{
		// 1. Background
		float bgImageW = (float)bgTex->GetWidth();
		float bgImageH = (float)bgTex->GetHeight();
		if (!bgSprite) {
			bgSprite = std::make_shared<DX9GF::StaticSprite>(bgTex.get());
			bgSprite->SetSrcRect({ 0, 0, (LONG)bgImageW, (LONG)bgImageH });
		}
		bgSprite->SetScale(std::max(screenW / bgImageW, screenH / bgImageH));
		bgSprite->SetOrigin(bgImageW / 2.0f, bgImageH / 2.0f);
		bgSprite->SetPosition(0, 0);

		// 2. Title
		float titleImageW = (float)titleTex->GetWidth();
		float titleImageH = (float)titleTex->GetHeight();
		if (!titleSprite) {
			titleSprite = std::make_shared<DX9GF::StaticSprite>(titleTex.get());
			titleSprite->SetSrcRect({ 0, 0, (LONG)titleImageW, (LONG)titleImageH });
		}
		titleSprite->SetScale(std::min((screenW * 0.5f) / titleImageW, (screenH * 0.4f) / titleImageH));
		titleSprite->SetOrigin(titleImageW / 2.0f, titleImageH / 2.0f);
		titleSprite->SetPosition(0, -screenH * 0.20f);

		// 3. UI Elements (Dùng hằng số)
		float startY = -screenH * 0.02f;

		// Hàm cục bộ giúp set vị trí Row Slider cho gọn
		auto SetVolumeRowPosition = [&](std::shared_ptr<DX9GF::StaticSprite> track, std::shared_ptr<DX9GF::StaticSprite> fill,
			std::shared_ptr<IconButton> btnD, std::shared_ptr<IconButton> btnI, float y) {

				float gap = 5.0f;       // Cố định khoảng cách 2 bên đều là 5 pixel
				float btnWidth = 12.0f; // Bề ngang của cái nút (theo đúng SrcRect cậu cắt)

				if (track) { track->SetPosition(SLIDER_COLUMN_X, y + ALIGN_OFFSET_Y); track->SetScale(SLIDER_SCALE_X, 1.0f); }
				if (fill) fill->SetPosition(SLIDER_COLUMN_X, y + ALIGN_OFFSET_Y);

				// Lùi về trái = Đúng chiều rộng của nút + gap
				if (btnD) btnD->SetLocalPosition(SLIDER_COLUMN_X - btnWidth - gap, y + ALIGN_OFFSET_Y - 2.0f);

				// Đẩy sang phải = Đúng chiều dài của track + gap
				if (btnI) btnI->SetLocalPosition(SLIDER_COLUMN_X + SLIDER_DESIRED_WIDTH + gap, y + ALIGN_OFFSET_Y - 2.0f);
			};

		SetVolumeRowPosition(trackMaster, trackMasterFill, btnMasterDec, btnMasterInc, startY);
		SetVolumeRowPosition(trackMusic, trackMusicFill, btnMusicDec, btnMusicInc, startY + SPACING_Y);
		SetVolumeRowPosition(trackSFX, trackSFXFill, btnSFXDec, btnSFXInc, startY + SPACING_Y * 2);

		if (backButton) backButton->SetLocalPosition(-(backButton->GetWidth() / 2.0f), -screenH * 0.10f);

		if (btnUp) btnUp->SetLocalPosition(SLIDER_COLUMN_X, startY + SPACING_Y * 3.5f - 3.0f);
		if (btnDown) btnDown->SetLocalPosition(SLIDER_COLUMN_X, startY + SPACING_Y * 5.0f - 3.0f);
		if (btnLeft) btnLeft->SetLocalPosition(SLIDER_COLUMN_X, startY + SPACING_Y * 6.5f - 3.0f);
		if (btnRight) btnRight->SetLocalPosition(SLIDER_COLUMN_X, startY + SPACING_Y * 8.0f - 3.0f);
	}

	void SettingsScene::Init()
	{
		transformManager = std::make_shared<DX9GF::TransformManager>();
		auto app = DX9GF::Application::GetInstance();
		lastScreenWidth = app->GetScreenWidth();
		lastScreenHeight = app->GetScreenHeight();

		// Load assets
		DX9GF::Font::AddFont(L"arcade-among-2-r46pv.ttf");
		font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arcade Among 2 R46PV", 24, 0, FW_BOLD);

		uiSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice()); uiSheetTex->LoadTexture(L"ui-pack.png");
		bgTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice()); bgTex->LoadTexture(IDB_PNG2);
		titleTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice()); titleTex->LoadTexture(IDB_PNG3);

		// Hàm hỗ trợ khởi tạo Track và Fill
		auto InitTrack = [&](std::shared_ptr<DX9GF::StaticSprite>& track, std::shared_ptr<DX9GF::StaticSprite>& fill, RECT trackR, RECT fillR) {
			track = std::make_shared<DX9GF::StaticSprite>(uiSheetTex.get()); track->SetSrcRect(trackR);
			fill = std::make_shared<DX9GF::StaticSprite>(uiSheetTex.get()); fill->SetSrcRect(fillR);
			};

		InitTrack(trackMaster, trackMasterFill, { 113, 483, 160, 490 }, { 65, 483, 112, 490 });
		InitTrack(trackMusic, trackMusicFill, { 113, 499, 160, 506 }, { 65, 499, 112, 506 });
		InitTrack(trackSFX, trackSFXFill, { 113, 515, 160, 522 }, { 65, 515, 112, 522 });

		// Hàm hỗ trợ khởi tạo Nút Cộng/Trừ (Tránh lặp code)
		auto CreateVolBtn = [&](int srcX, int srcY, const std::function<void(DX9GF::ITrigger*)>& action) {
			auto btn = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 12, 11, uiSheetTex, 1);
			btn->SetSpriteCoords(srcX, srcY, 12, 11, 0);

			// --- SỬA Ở ĐÂY: Bọc cái action lại và gọi thêm hàm SaveSettings ---
			btn->SetOnReleaseLeft([action](DX9GF::ITrigger* t) {
				action(t); // Thực thi việc tăng/giảm Volume
				SettingsManager::GetInstance()->SaveSettings(); // LƯU NGAY LẬP TỨC!
				});			
			return btn;
			};

		auto sm = SettingsManager::GetInstance();
		btnMasterDec = CreateVolBtn(82, 82, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetMasterVolume(std::max(0.0f, SettingsManager::GetInstance()->GetMasterVolume() - 0.2f)); });
		btnMasterInc = CreateVolBtn(82, 71, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetMasterVolume(std::min(1.0f, SettingsManager::GetInstance()->GetMasterVolume() + 0.2f)); });

		btnMusicDec = CreateVolBtn(82, 82, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetMusicVolume(std::max(0.0f, SettingsManager::GetInstance()->GetMusicVolume() - 0.2f)); });
		btnMusicInc = CreateVolBtn(82, 71, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetMusicVolume(std::min(1.0f, SettingsManager::GetInstance()->GetMusicVolume() + 0.2f)); });

		btnSFXDec = CreateVolBtn(82, 82, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetSfxVolume(std::max(0.0f, SettingsManager::GetInstance()->GetSfxVolume() - 0.2f)); });
		btnSFXInc = CreateVolBtn(82, 71, [](DX9GF::ITrigger*) { SettingsManager::GetInstance()->SetSfxVolume(std::min(1.0f, SettingsManager::GetInstance()->GetSfxVolume() + 0.2f)); });

		// Nút điều hướng & Back
		backButton = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 62, 30, uiSheetTex, 3);
		backButton->SetSpriteCoords(1, 417, 62, 30, 2);
		backButton->SetOnReleaseLeft([this](DX9GF::ITrigger*) { this->isGoingBack = true; });

		//Use the same placeholder image for all control buttons for now.
		btnUp = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 30, 30, uiSheetTex, 2);
		btnUp->SetSpriteCoords(1, 481, 30, 30, 2);
		btnUp->SetOnReleaseLeft([this](DX9GF::ITrigger*) 
			{
				this->ResetListening();
				this->isListeningUp = true;
				btnUp->SetState(Demo::IButton::ButtonState::LISTENING);
			});

		btnDown = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 30, 30, uiSheetTex, 2);
		btnDown->SetSpriteCoords(1, 481, 30, 30, 2);
		btnDown->SetOnReleaseLeft([this](DX9GF::ITrigger*)
			{
				this->ResetListening();
				this->isListeningDown = true;
				btnDown->SetState(Demo::IButton::ButtonState::LISTENING);
			});

		btnLeft = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 30, 30, uiSheetTex, 2);
		btnLeft->SetSpriteCoords(1, 481, 30, 30, 2);
		btnLeft->SetOnReleaseLeft([this](DX9GF::ITrigger*)
			{
				this->ResetListening();
				this->isListeningLeft = true;
				btnLeft->SetState(Demo::IButton::ButtonState::LISTENING);
			});

		btnRight = std::make_shared<Demo::IconButton>(transformManager, 0, 0, 30, 30, uiSheetTex, 2);
		btnRight->SetSpriteCoords(1, 481, 30, 30, 2);
		btnRight->SetOnReleaseLeft([this](DX9GF::ITrigger*)
			{
				this->ResetListening();
				this->isListeningRight = true;
				btnRight->SetState(Demo::IButton::ButtonState::LISTENING);
			});



		// Active Buttons
		std::shared_ptr<Demo::IButton> buttons[] = { backButton, btnUp, btnDown, btnLeft,btnRight, btnMasterDec, btnMasterInc, btnMusicDec, btnMusicInc, btnSFXDec, btnSFXInc };
		for (auto& btn : buttons) 
		{
			if (btn)
			{
				btn->Init(&camera);
				uiButtons.push_back(btn);
			}
		}

		UpdateLayout(lastScreenWidth, lastScreenHeight);
		transformManager->RebuildHierarchy();
	}

	void SettingsScene::Update(unsigned long long deltaTime)
	{
		auto inpMan = DX9GF::InputManager::GetInstance();
		inpMan->ReadMouse(deltaTime);
		inpMan->ReadKeyboard(deltaTime);

		auto app = DX9GF::Application::GetInstance();
		if (app->GetScreenWidth() != lastScreenWidth || app->GetScreenHeight() != lastScreenHeight) {
			lastScreenWidth = app->GetScreenWidth();
			lastScreenHeight = app->GetScreenHeight();
			UpdateLayout(lastScreenWidth, lastScreenHeight);
			font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), L"Arcade Among 2 R46PV", 24, 0, FW_BOLD);
		}

		for (auto& button : uiButtons) button->Update(deltaTime);

		transformManager->UpdateAll();
		camera.Update();

		if (isListeningUp) 
		{
			for (int i = 0x07; i <= 0xFE; i++) 
			{
				if (GetAsyncKeyState(i) & 0x8000) 
				{
					if (i >= 1 && i <= 6) continue;
					SettingsManager::GetInstance()->SetKeybind("MOVE_UP", i);
					btnUp->SetState(IButton::ButtonState::IDLE);
					isListeningUp = false;
					SettingsManager::GetInstance()->SaveSettings();
					break;
				}
			}
		}

		if (isListeningDown) 
		{
			for (int i = 0x07; i <= 0xFE; i++) 
			{
				if (GetAsyncKeyState(i) & 0x8000) 
				{
					if (i >= 1 && i <= 6) continue;
					SettingsManager::GetInstance()->SetKeybind("MOVE_DOWN", i);
					btnDown->SetState(IButton::ButtonState::IDLE);
					isListeningUp = false;
					SettingsManager::GetInstance()->SaveSettings();
					break;
				}
			}
		}

		if (isListeningLeft) 
		{
			for (int i = 0x07; i <= 0xFE; i++) 
			{
				if (GetAsyncKeyState(i) & 0x8000) 
				{
					if (i >= 1 && i <= 6) continue;
					SettingsManager::GetInstance()->SetKeybind("MOVE_LEFT", i);
					btnLeft->SetState(IButton::ButtonState::IDLE);
					isListeningUp = false;
					SettingsManager::GetInstance()->SaveSettings();
					break;
				}
			}
		}

		if (isListeningRight) 
		{
			for (int i = 0x07; i <= 0xFE; i++) 
			{
				if (GetAsyncKeyState(i) & 0x8000) 
				{
					if (i >= 1 && i <= 6) continue;
					SettingsManager::GetInstance()->SetKeybind("MOVE_RIGHT", i);
					btnRight->SetState(IButton::ButtonState::IDLE);
					isListeningUp = false;
					SettingsManager::GetInstance()->SaveSettings();
					break;
				}
			}
		}

		if (this->isGoingBack) {
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
			if (bgSprite) { bgSprite->Begin(); bgSprite->Draw(camera, deltaTime); bgSprite->End(); }
			if (titleSprite) { titleSprite->Begin(); titleSprite->Draw(camera, deltaTime); titleSprite->End(); }

			float startY = -lastScreenHeight * 0.02f;

			// Vẽ Label Text
			DrawString(L"MASTER VOLUME", LABEL_COLUMN_X, startY, D3DCOLOR_XRGB(255, 255, 255));
			DrawString(L"MUSIC VOLUME", LABEL_COLUMN_X, startY + SPACING_Y, D3DCOLOR_XRGB(255, 255, 255));
			DrawString(L"SFX VOLUME", LABEL_COLUMN_X, startY + SPACING_Y * 2, D3DCOLOR_XRGB(255, 255, 255));
			DrawString(L"MOVE UP", LABEL_COLUMN_X, startY + SPACING_Y * 3.5f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE DOWN", LABEL_COLUMN_X, startY + SPACING_Y * 5.0f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE LEFT", LABEL_COLUMN_X, startY + SPACING_Y * 6.5f, D3DCOLOR_XRGB(200, 200, 255));
			DrawString(L"MOVE RIGHT", LABEL_COLUMN_X, startY + SPACING_Y * 8.0f, D3DCOLOR_XRGB(200, 200, 255));

			// Vẽ Tracks
			auto sm = SettingsManager::GetInstance();
			DrawVolumeTrack(trackMaster, trackMasterFill, sm->GetMasterVolume(), { 65, 483, 112, 490 }, deltaTime);
			DrawVolumeTrack(trackMusic, trackMusicFill, sm->GetMusicVolume(), { 65, 499, 112, 506 }, deltaTime);
			DrawVolumeTrack(trackSFX, trackSFXFill, sm->GetSfxVolume(), { 65, 515, 112, 522 }, deltaTime);

			// Vẽ Buttons
			for (auto& btn : uiButtons) btn->Draw(&this->camera, gd, deltaTime);

			// Vẽ Text trên Button Keybind
			DrawKeybindButton("MOVE_UP", btnUp, isListeningUp);
			DrawKeybindButton("MOVE_DOWN", btnDown, isListeningDown);
			DrawKeybindButton("MOVE_LEFT", btnLeft, isListeningLeft);
			DrawKeybindButton("MOVE_RIGHT", btnRight, isListeningRight);

			gd->EndDraw();
		}
		gd->Present();
	}
}