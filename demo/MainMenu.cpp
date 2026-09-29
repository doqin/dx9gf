#include "pch.h"
#include "MainMenu.h"
#include "resource.h"
#include "IconButton.h"
#include "SettingsScene.h"
#include "ThreadAlleyScene.h"
#include "TutorialWorldScene.h"
#include "SecretPuzzleScene.h"
#include "BossWorldScene.h"
#include "SaveGameState.h"
#include <fstream>
#include <cstdio>
#include "TransitionCommand.h"
#include "CreditsScene.h"
#include "PopupManager.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include "PendingAutoContinue.h"

namespace Demo
{
	class WhiteFadeOutCommand : public DX9GF::ICommand {
		DX9GF::GraphicsDevice* gd;
		DX9GF::Camera* camera;
		float duration;
		float timer = 0.0f;
		int vw, vh;
	public:
		WhiteFadeOutCommand(DX9GF::GraphicsDevice* gd, DX9GF::Camera* camera, float duration, int vw, int vh) 
			: gd(gd), camera(camera), duration(duration), vw(vw), vh(vh) {}
		void Execute(unsigned long long deltaTime) override {
			if (IsFinished()) return;
			timer += deltaTime / 1000.0f;
			if (timer >= duration) {
				timer = duration;
				MarkFinished();
			}
			float alpha = 1.0f - (timer / duration);
			int a = std::clamp(static_cast<int>(alpha * 255.0f), 0, 255);
			if (a > 0) {
				float startX = -vw / 2.0f;
				float startY = -vh / 2.0f;
				gd->SetAlphaBlending(true);
				gd->DrawRectangle(*camera, startX, startY, static_cast<float>(vw), static_cast<float>(vh), D3DCOLOR_ARGB(a, 255, 255, 255), true);
				gd->SetAlphaBlending(false);
			}
		}
	};

	std::shared_ptr<SaveGameState> MainMenu::gameSaveState = nullptr;
	void MainMenu::UpdateLayout(int screenW, int screenH)
	{
		// BACKGROUND - use aspect fill
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

		// Draw UI: Anchor to the left edge of the virtual screen to keep it clear of the black bars
		float spacingY = 10.0f;
		float startY = -screenH * 0.10f;
		float currentY = startY;

		// -screenW / 2.f is the left edge of the gameplay area, +64.f offsets it inward
		float leftAnchorX = -screenW / 2.f + 64.f;

		std::shared_ptr<Demo::IButton> buttons[] = { continueButton, newGameButton, optionsButton, creditsButton, quitButton };

		for (auto& btn : buttons)
		{
			if (btn)
			{
				btn->SetLocalPosition(leftAnchorX, currentY);
				currentY += btn->GetHeight() + spacingY;
			}
		}

		// TITLE
		auto [_, y] = continueButton->GetLocalPosition();
		auto height = fontSprite->GetHeight();
		fontSprite->SetPosition(leftAnchorX, y - height * 3 * 2.f - 32.f);
	}

	void MainMenu::RenderMapPreviews()
	{
		const UINT PREVIEW_SIZE = 256;
		const float PREVIEW_ZOOM = 1.0f;
		auto gd = game->GetGraphicsDevice();

		auto captureOne = [&](int index, Demo::WorldSceneBase* scene, float focusX, float focusY, float zoom) {
			scene->Init();
			scene->GetCamera().SetPosition(focusX, focusY);
			scene->GetCamera().SetZoom(zoom);

			mapPreviews[index] = std::make_shared<DX9GF::Texture>(gd);
			mapPreviews[index]->CreateRenderTarget(PREVIEW_SIZE, PREVIEW_SIZE);

			gd->SetRenderTarget(mapPreviews[index].get());
			gd->SetViewport(0, 0, PREVIEW_SIZE, PREVIEW_SIZE, 0.0f, 1.0f);
			gd->ResetVirtualTransform();
			gd->Clear(0xFF000000);
			scene->DrawWorld(0);
			gd->RestoreRenderTarget();

			delete scene;
			};

		// focus points are each map's player spawn location, from its scene's InitCore() call
		captureOne(0, new TutorialWorldScene(game, saveManager, PREVIEW_SIZE, PREVIEW_SIZE), 248.0f, 184.0f, PREVIEW_ZOOM);
		captureOne(1, new SecretPuzzleScene(game, saveManager, PREVIEW_SIZE, PREVIEW_SIZE), -84.0f * 16.0f, -39.0f * 16.0f, PREVIEW_ZOOM);
		captureOne(2, new ThreadAlleyScene(game, saveManager, PREVIEW_SIZE, PREVIEW_SIZE), -544.5f, 128.5f, PREVIEW_ZOOM);
		captureOne(3, new BossWorldScene(game, saveManager, PREVIEW_SIZE, PREVIEW_SIZE), 360.0f, 190.0f, PREVIEW_ZOOM);

		gd->SetViewport(0, 0, lastScreenWidth, lastScreenHeight, 0.0f, 1.0f);
	}

	void MainMenu::DrawBackground(unsigned long long deltaTime)
	{
		auto [screenWidth, screenHeight] = camera.GetScreenResolution();
		auto gd = game->GetGraphicsDevice();

		gd->DrawRectangle(0.0f, 0.0f, static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0xFF242234, true);

		const D3DCOLOR polyColor = 0xFF9cdb43;
		const int RING_COUNT = 3;
		const float TWO_PI_OVER_5 = 2.0f * 3.14159f / 5.0f;

		//shared static timer variable for background processes
		static float timeAcc = 0.0f;
		timeAcc += static_cast<float>(deltaTime) * 0.001f;

		// filler pentagons: a dense starfield/galaxy of tiny drifting wireframe specks behind
		// the map pentagons - mostly small and dim (distant "stars"), a few bigger and
		// brighter (closer "nebulae"), all gently twinkling in opacity
		const int FILLER_COUNT = 90;
		gd->SetAlphaBlending(true);
		for (int i = 0; i < FILLER_COUNT; ++i) {
			float sizeFactor = static_cast<float>(i % 9); // 0..8, most speck sizes repeat often
			float size = 6.0f + sizeFactor * 6.0f; // 6..54
			float margin = size * 2.0f;

			// parallax: smaller specks drift slower (feel farther away), bigger ones faster
			float speedScale = 0.3f + (size / 54.0f) * 1.3f;
			float driftX = 6.0f * speedScale;
			float driftY = 4.0f * speedScale;

			float bx = std::fmod((i * 191.0f) + timeAcc * driftX, static_cast<float>(screenWidth) + margin * 2.0f) - margin;
			float by = std::fmod((i * 337.0f) + timeAcc * driftY, static_cast<float>(screenHeight) + margin * 2.0f) - margin;
			float angle = timeAcc * (0.05f + 0.1f * speedScale) + i * 0.7f;
			float glitchSize = size + std::sinf(timeAcc * 1.5f + i) * (size * 0.08f);

			int baseAlpha = 6 + static_cast<int>(sizeFactor) * 4;
			int alpha = std::clamp(static_cast<int>(baseAlpha + std::sinf(timeAcc * 1.3f + i * 2.1f) * 8.0f), 3, 55);
			D3DCOLOR fillerColor = D3DCOLOR_ARGB(alpha, 0x9c, 0xdb, 0x43);

			float prevX = bx + std::cosf(angle) * glitchSize;
			float prevY = by + std::sinf(angle) * glitchSize;
			for (int v = 1; v <= 5; ++v) {
				float vAngle = angle + v * TWO_PI_OVER_5;
				float vx = bx + std::cosf(vAngle) * glitchSize;
				float vy = by + std::sinf(vAngle) * glitchSize;

				gd->DrawLine(prevX, prevY, vx, vy, fillerColor);
				prevX = vx;
				prevY = vy;
			}
		}
		gd->SetAlphaBlending(false);

		// shared wrap margin/period for every pentagon so their start phases can be evenly
		// spread across the cycle - keeps them spaced apart instead of clumping together
		const float margin = 300.0f;
		const float periodX = static_cast<float>(screenWidth) + margin * 2.0f;
		const float periodY = static_cast<float>(screenHeight) + margin * 2.0f;

		for (int i = 0; i < MAP_PREVIEW_COUNT; ++i) {
			float baseSize = 130.0f + (i % 3) * 30.0f;

			// evenly space starting phases 1/MAP_PREVIEW_COUNT of a cycle apart; y uses a
			// different permutation of the index so the four don't move as a rigid grid
			int yIndex = (i * 3) % MAP_PREVIEW_COUNT;
			float phaseX = (static_cast<float>(i) / MAP_PREVIEW_COUNT) * periodX;
			float phaseY = (static_cast<float>(yIndex) / MAP_PREVIEW_COUNT) * periodY;

			float cx = std::fmod(phaseX + timeAcc * 28.0f, periodX) - margin;
			float cy = std::fmod(phaseY + timeAcc * 16.0f, periodY) - margin;
			float angle = timeAcc * 0.1f + i;
			float size = baseSize + std::sinf(timeAcc * 2.0f + i) * 5.0f;

			std::vector<D3DXVECTOR2> points;
			std::vector<D3DXVECTOR2> uvs;
			points.reserve(5);
			uvs.reserve(5);

			// Binocular effect, without ever running out of texture: the on-screen pentagon
			// shape still tumbles (points, using `angle`), but the sampled crop uses its own
			// fixed-orientation window that does NOT rotate with it, so the backdrop doesn't
			// spin along with the frame. The crop's pan position follows this pentagon's own
			// wrap-cycle (panU/panV cycle smoothly through 0..1 as cx/cy wrap), so it always
			// stays in valid texture range instead of clamping once far from screen center.
			float panU = (cx + margin) / periodX;
			float panV = (cy + margin) / periodY;
			const float uvRadius = 0.3f; // fraction of texture shown per pentagon - raise to zoom out, lower to zoom in (keep under ~0.5 to avoid clamped edges)

			for (int v = 0; v < 5; ++v) {
				float vAngle = angle + v * TWO_PI_OVER_5;
				float vx = cx + std::cosf(vAngle) * size;
				float vy = cy + std::sinf(vAngle) * size;
				points.push_back(D3DXVECTOR2(vx, vy));

				float canonicalAngle = v * TWO_PI_OVER_5; // no `angle` term - keeps the crop from spinning with the window
				uvs.push_back(D3DXVECTOR2(
					panU + std::cosf(canonicalAngle) * uvRadius,
					panV + std::sinf(canonicalAngle) * uvRadius));
			}

			if (mapPreviews[i]) {
				gd->DrawTexturedPolygon(points, uvs, mapPreviews[i].get());
			}

			for (int ring = 0; ring < RING_COUNT; ++ring) {
				float ringSize = size * (1.0f + ring * 0.22f);
				float prevX = cx + std::cosf(angle) * ringSize;
				float prevY = cy + std::sinf(angle) * ringSize;

				for (int v = 1; v <= 5; ++v) {
					float vAngle = angle + v * TWO_PI_OVER_5;
					float vx = cx + std::cosf(vAngle) * ringSize;
					float vy = cy + std::sinf(vAngle) * ringSize;

					gd->DrawLine(prevX, prevY, vx, vy, polyColor);
					prevX = vx;
					prevY = vy;
				}
			}
		}
	}

	void MainMenu::Init()
	{
		drawBuffer = std::make_shared<DX9GF::CommandBuffer>();
		commandBuffer = std::make_shared<DX9GF::CommandBuffer>();
		transformManager = std::make_shared<DX9GF::TransformManager>();
		saveManager = std::make_shared<DX9GF::SaveManager>();
		gameSaveState = std::make_shared<SaveGameState>(game, saveManager);

		auto [camW, camH] = camera.GetScreenResolution();
		lastScreenWidth = camW;
		lastScreenHeight = camH;

		//load textures
		buttonSheetTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		buttonSheetTex->LoadTexture(L"assets/ui.png");

		bgTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		bgTex->LoadTexture(IDB_PNG2);

		titleTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		titleTex->LoadTexture(IDB_PNG3);


		// Sprites
		font = std::make_shared<DX9GF::Font>(game->GetGraphicsDevice(), Demo::kMainFontName, Demo::kMainFontSize);
		fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
		fontSprite->SetColor(0xFF000000);

		//LOCAL FUNCTION to init a menu text-icon button: blank slice-able frame + auto-sizing translated label
		auto InitMenuButton = [&](std::shared_ptr<Demo::TextIconButton>& btn, const std::wstring& english) {
			btn = std::make_shared<Demo::TextIconButton>(transformManager, 0, 0, 96, 32, buttonSheetTex, font.get(), L"", 3);
			btn->SetSpriteRects(DX9GF::Utils::CreateRectsVertical(0, 0, 16, 16, 3));
			btn->SetSliceMargins(4, 4);
			btn->SetSpriteScale(2.f, 2.f);
			btn->SetAutoResize(true, 16.f);
			btn->SetTextScale(1.f, 1.f);
			btn->SetTextColor(D3DCOLOR_XRGB(0, 0, 0));
			btn->SetTextOutline(false);
			btn->SetDynamicTextGetter([english]() { return Tr(english); });
			};

		//BUTTONS INIT
		//Continue Button
		InitMenuButton(continueButton, L"Continue");
		/*continueButton->SetState(IButton::ButtonState::DISABLED);*/

		auto borderTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		borderTex->LoadTexture(L"assets/popup-borders.png");

		auto uiTex = std::make_shared<DX9GF::Texture>(game->GetGraphicsDevice());
		uiTex->LoadTexture(L"assets/ui.png");

		PopupManager::GetInstance()->Init(game, borderTex, uiTex, font);

		std::ifstream f("savegame.json");
		bool hasSave = f.good();
		if (hasSave) {
			continueButton->SetState(IButton::ButtonState::IDLE);
		}
		else {
			continueButton->SetState(IButton::ButtonState::DISABLED);
		}
		f.close();

		doContinueGame = [this]() {
			if (isTransitioning) return;
			isTransitioning = true;
			auto transitionInCommand = std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, true);
			drawBuffer->PushCommand(transitionInCommand);
			commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this, transitionInCommand](std::function<void(void)> markFinished) {
				if (!transitionInCommand->IsFinished()) {
					return;
				}
				gameSaveState = SaveGameState::LoadSavedGame(game, saveManager);
				isTransitioning = false;
				markFinished();
				}));
			drawBuffer->PushCommand(std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, false));
			};


		continueButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
			doContinueGame();
			});

		//New Game Button
		InitMenuButton(newGameButton, L"New Game");
		newGameButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
			if (isTransitioning) return;

			//check save file
			std::ifstream f("savegame.json");
			bool hasSave = f.good();
			f.close();

			auto startNewGameLogic = [this]() {
				this->isTransitioning = true;
				auto transitionInCommand = std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, true);

				this->drawBuffer->PushCommand(transitionInCommand);
				this->commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this, transitionInCommand](std::function<void(void)> markFinished) {
					if (!transitionInCommand->IsFinished()) return;

					std::remove("savegame.json");
					gameSaveState = SaveGameState::StartNewGame(this->game, this->saveManager);
					this->isTransitioning = false;

					this->commandBuffer->PushCommand(std::make_shared<DX9GF::CustomCommand>([this](std::function<void(void)> markFinished1) {
						std::ifstream f2("savegame.json");
						if (f2.good()) this->continueButton->SetState(IButton::ButtonState::IDLE);
						else this->continueButton->SetState(IButton::ButtonState::DISABLED);
						f2.close();

						markFinished1();
						}));
					markFinished();
					}));
				this->drawBuffer->PushCommand(std::make_shared<TransitionCommand>(game, &this->uiCamera, 1.f, false));
				};

			if (hasSave) {
				std::vector<std::pair<std::wstring, std::function<void()>>> popupBtns = {
					{ Tr(L"Yes"), startNewGameLogic },
					{ Tr(L"No"), []() {} }
				};
				PopupManager::GetInstance()->Show("stepped_red", Tr(L"WARNING"), Tr(L"Overwrite existing save?"), popupBtns);
			}
			else {
				startNewGameLogic();
			}
			});


		//Options Button
		InitMenuButton(optionsButton, L"Options");
		optionsButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
			auto app = DX9GF::Application::GetInstance();
			//push Settings Scene
			auto sceMan = this->game->GetSceneManager();
			sceMan->InsertScene(sceMan->GetIndex() + 1,
				new SettingsScene(this->game, app->GetScreenWidth(), app->GetScreenHeight())
			);
			this->game->GetSceneManager()->GoToNext();
			});

		//Credits Button
		InitMenuButton(creditsButton, L"Credits");
		creditsButton->SetOnReleaseLeft([this](DX9GF::ITrigger* t) {
			auto app = DX9GF::Application::GetInstance();
			auto sceMan = this->game->GetSceneManager();
			sceMan->InsertScene(sceMan->GetIndex() + 1,
				new CreditsScene(this->game, app->GetScreenWidth(), app->GetScreenHeight())
			);
			this->game->GetSceneManager()->GoToNext();
			});

		//Quit Button
		InitMenuButton(quitButton, L"Quit");
		quitButton->SetOnReleaseLeft([](DX9GF::ITrigger* t) { PostQuitMessage(0); });

		//active buttons
		std::shared_ptr<Demo::IButton> buttons[] = { continueButton, newGameButton, optionsButton, creditsButton, quitButton };
		for (auto& btn : buttons)
		{
			if (btn)
			{
				btn->Init(&uiCamera);
				uiButtons.push_back(btn);
			}
		}

		auto audio = DX9GF::AudioManager::GetInstance();

		audio->Load("bgm_sky", IDR_BGM_SKY);
		audio->PlayBGM_Fade("bgm_sky", 0.9f, 1.5f);

		//render one-off snapshots of the featured maps for the background pentagons
		RenderMapPreviews();

		//call it to setup the update layout
		UpdateLayout(lastScreenWidth, lastScreenHeight);
		transformManager->RebuildHierarchy();
		
		// Queue a white fade out when MainMenu starts to blend from SplashScene
		drawBuffer->PushCommand(std::make_shared<WhiteFadeOutCommand>(game->GetGraphicsDevice(), &this->uiCamera, 1.5f, game->GetVirtualWidth(), game->GetVirtualHeight()));
	}

	std::vector<KeyboardNavigator::Candidate> MainMenu::CollectKeyboardCandidates()
	{
		std::vector<KeyboardNavigator::Candidate> candidates;
		for (auto& button : uiButtons) {
			if (!button || button->GetState() == IButton::ButtonState::DISABLED) {
				continue;
			}
			candidates.push_back({
				button,
				button->GetWorldX(),
				button->GetWorldY(),
				(float)button->GetWidth(),
				(float)button->GetHeight(),
				[button]() { button->Activate(); }
				});
		}
		return candidates;
	}

	void MainMenu::Update(unsigned long long deltaTime)
	{
		PopupManager::GetInstance()->SetUICamera(&this->uiCamera);

		if (doContinueGame && Demo::PendingAutoContinue::GetInstance()->ConsumeIfPending()) {
			std::ifstream f("savegame.json");
			bool hasSave = f.good();
			f.close();
			if (hasSave) {
				doContinueGame();
			}
		}

		auto inpMan = DX9GF::InputManager::GetInstance();
		inpMan->ReadMouse(deltaTime);
		inpMan->ReadKeyboard(deltaTime);

		auto [currW, currH] = camera.GetScreenResolution();
		UpdateLayout(currW, currH);

		if (!PopupManager::GetInstance()->IsActive()) {
			for (auto& button : uiButtons)
			{
				if (button->GetState() == IButton::ButtonState::DISABLED) continue;
				button->Update(deltaTime);
			}
			keyboardNavigator.Update(deltaTime, CollectKeyboardCandidates());
		}
		else {
			keyboardNavigator.Reset();
		}
		PopupManager::GetInstance()->Update(deltaTime, &this->uiCamera);
		transformManager->UpdateAll();
		camera.Update();
		commandBuffer->Update(deltaTime);
	}

	void MainMenu::DrawWorld(unsigned long long deltaTime)
	{
		auto gd = game->GetGraphicsDevice();

		if (SUCCEEDED(gd->BeginDraw())) {
			DrawBackground(deltaTime);
			gd->EndDraw();
		}
	}

	void MainMenu::DrawUI(unsigned long long deltaTime)
	{
		auto gd = game->GetGraphicsDevice();

		if (SUCCEEDED(gd->BeginDraw())) {

			fontSprite->Begin();
			fontSprite->SetScale(2.f, 2.f);
			auto prevPos = fontSprite->GetPosition();
			auto height = fontSprite->GetHeight() * 2.f;

			fontSprite->SetColor(0xFFFFFFFF);
			fontSprite->SetOutline(true, 0xFF000000, 2.0f);

			fontSprite->SetText(Tr(L"I, a 6th-year UIT student,"));
			fontSprite->Draw(uiCamera, deltaTime);

			fontSprite->SetPosition(prevPos.x, prevPos.y + height);
			fontSprite->SetText(Tr(L"got sucked into cyberspace"));
			fontSprite->Draw(uiCamera, deltaTime);

			fontSprite->SetPosition(prevPos.x, prevPos.y + height * 2);
			fontSprite->SetText(Tr(L"because I clicked a shady link"));
			fontSprite->Draw(uiCamera, deltaTime);

			fontSprite->SetPosition(prevPos.x, prevPos.y);
			fontSprite->End();

			for (auto& btn : uiButtons)
			{
				btn->Draw(gd, deltaTime);
			}

			keyboardNavigator.Draw(gd, uiCamera, CollectKeyboardCandidates());

			drawBuffer->Update(deltaTime);
			PopupManager::GetInstance()->DrawUI(deltaTime, &this->uiCamera);
			if (!keyboardNavigator.IsInKeyboardMode() && !PopupManager::GetInstance()->IsKeyboardNavigating()) {
				DX9GF::InputManager::GetInstance()->DrawCursor(&this->uiCamera, deltaTime);
			}
			gd->EndDraw();
		}
	}


}
