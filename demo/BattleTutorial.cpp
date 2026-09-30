#include "pch.h"
#include "BattleTutorial.h"
#include "LocalizationManager.h"
#include <algorithm>
#include <cmath>

namespace Demo {

	// Panel rows in assets/tutorial.png (192px wide sheet). Order matches the Step enum.
	const BattleTutorial::PanelRect BattleTutorial::PANELS[BattleTutorial::STEP_COUNT] = {
		{ 242, 322 },   // STEP_DRAG_CARDS
		{ 558, 674 },   // STEP_BLOCKS
		{ 799, 871 },   // STEP_ENERGY
		{ 878, 954 },   // STEP_NON_PERSISTENT
		{ 984, 1048 },  // STEP_USE_LIMIT
		{ 322, 401 },   // STEP_ENEMY_CARD
		{ 401, 498 },   // STEP_TARGET
		{ 498, 553 },   // STEP_EXECUTE
		{ 700, 786 },   // STEP_CYCLE
	};

	namespace {
		constexpr float PANEL_SHEET_WIDTH = 192.f;
		constexpr float PANEL_SCALE = 3.f;
		constexpr float OPEN_INPUT_COOLDOWN_MS = 200.f;

		constexpr float SKIP_SCALE = 3.f;
		constexpr float SKIP_W = 32.f * SKIP_SCALE;
		constexpr float SKIP_H = 16.f * SKIP_SCALE;
		constexpr float SKIP_MARGIN = 24.f;
	}

	BattleTutorial::BattleTutorial(DX9GF::GraphicsDevice* gd, DX9GF::Font* font)
		: device(gd), uiFont(font)
	{
		sheet = std::make_shared<DX9GF::Texture>(gd);
		sheet->LoadTexture(L"assets/tutorial.png");
		sprite = std::make_shared<DX9GF::StaticSprite>(sheet.get());
		sprite->SetScale(PANEL_SCALE, PANEL_SCALE);

		uiTex = std::make_shared<DX9GF::Texture>(gd);
		uiTex->LoadTexture(L"assets/ui.png");
	}

	void BattleTutorial::TryQueue(Step s, bool condition)
	{
		if (condition && !handled[s]) {
			handled[s] = true;
			queue.push_back(s);
			if (queue.size() == 1) {
				inputCooldown = OPEN_INPUT_COOLDOWN_MS;
				appearElapsed = 0.f;
			}
		}
	}

	void BattleTutorial::Observe(const BattleTutorialContext& ctx)
	{
		// The opening burst: the core of the programming loop, shown the first time the player
		// reaches the card-programming phase.
		if (ctx.inProgrammingPhase && ctx.currentTurn == 1) {
			TryQueue(STEP_DRAG_CARDS, true);
			TryQueue(STEP_BLOCKS, true);
			TryQueue(STEP_ENERGY, true);
		}

		// Contextual steps, each fired by the player getting far enough to need it.
		TryQueue(STEP_ENEMY_CARD, ctx.inProgrammingPhase && ctx.cardInBlock);
		TryQueue(STEP_TARGET, ctx.enemyCardExists);
		TryQueue(STEP_EXECUTE, ctx.enemyCardTargeted);

		const bool onTurnTwo = ctx.inProgrammingPhase && ctx.currentTurn >= 2;
		TryQueue(STEP_CYCLE, onTurnTwo);
		// Card-lifecycle notes: shown when such a card first turns up, otherwise held back
		// until turn 2 so they are never missed.
		TryQueue(STEP_NON_PERSISTENT, ctx.nonPersistentCardInHand || onTurnTwo);
		TryQueue(STEP_USE_LIMIT, ctx.limitedUseCardInHand || onTurnTwo);
	}

	void BattleTutorial::Update(unsigned long long deltaTime)
	{
		if (queue.empty()) return;

		const float dt = static_cast<float>(deltaTime);
		appearElapsed += dt;
		if (inputCooldown > 0.f) {
			inputCooldown -= dt;
			return;
		}

		auto inp = DX9GF::InputManager::GetInstance();

		if (skipBtn) {
			skipBtn->Update(deltaTime);
			if (skipRequested) {
				skipRequested = false;
				inp->ConsumeMouseButton(DX9GF::InputManager::MouseButton::Left);
				Skip();
				return;
			}
			// Pressing on Skip must not also advance to the next panel.
			const auto st = skipBtn->GetState();
			if (st == IButton::ButtonState::HOVER || st == IButton::ButtonState::CLICKED) return;
		}

		const bool dismiss =
			inp->MouseDown(DX9GF::InputManager::MouseButton::Left) ||
			inp->KeyDown(DIK_SPACE) ||
			inp->KeyDown(DIK_RETURN);
		if (!dismiss) return;

		inp->ConsumeMouseButton(DX9GF::InputManager::MouseButton::Left);
		queue.pop_front();
		if (!queue.empty()) {
			inputCooldown = OPEN_INPUT_COOLDOWN_MS;
			appearElapsed = 0.f;
		}
	}

	void BattleTutorial::Draw(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera,
		DX9GF::FontSprite* fontSprite, float screenW, float screenH,
		unsigned long long deltaTime)
	{
		if (queue.empty()) {
			return;
		}
		const PanelRect& r = PANELS[queue.front()];
		const float panelH = static_cast<float>(r.bottom - r.top);
		const float drawnH = panelH * PANEL_SCALE;

		gd->SetAlphaBlending(true);
		gd->DrawRectangle(uiCamera, -screenW / 2.f, -screenH / 2.f, screenW, screenH,
			D3DCOLOR_ARGB(190, 0, 0, 0), true);

		sprite->SetSrcRect(RECT{ 0, r.top, static_cast<LONG>(PANEL_SHEET_WIDTH), r.bottom });
		sprite->SetOrigin(PANEL_SHEET_WIDTH / 2.f, panelH / 2.f);
		sprite->SetPosition(0.f, -18.f);
		sprite->Begin();
		sprite->Draw(uiCamera, deltaTime);
		sprite->End();

		// Pulsing "click to continue" hint below the panel.
		const int hintAlpha = 150 + static_cast<int>(90.0 * (0.5 + 0.5 * std::sin(appearElapsed * 0.006)));
		fontSprite->SetScale(1.f, 1.f);
		fontSprite->SetText(Tr(L"Click / Space to continue"));
		fontSprite->SetColor(D3DCOLOR_ARGB((std::min)(255, hintAlpha), 255, 255, 255));
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		fontSprite->SetPosition(-fontSprite->GetWidth() / 2.f, -18.f + drawnH / 2.f + 18.f);
		fontSprite->Begin();
		fontSprite->Draw(uiCamera, deltaTime);
		fontSprite->End();
		fontSprite->SetOutline(false);
		fontSprite->SetColor(0xFFFFFFFF);

		// Skip button, bottom-centre.
		EnsureSkipButton(uiCamera);
		skipBtn->SetLocalPosition(-SKIP_W / 2.f, screenH / 2.f - SKIP_H - SKIP_MARGIN);
		uiTransformManager->UpdateAll();
		skipBtn->Draw(gd, deltaTime);

		gd->SetAlphaBlending(false);
	}

	void BattleTutorial::ReplayAll()
	{
		// Teaching order, not enum order: the lifecycle notes come last.
		static const Step order[] = {
			STEP_DRAG_CARDS, STEP_BLOCKS, STEP_ENERGY,
			STEP_ENEMY_CARD, STEP_TARGET, STEP_EXECUTE, STEP_CYCLE,
			STEP_NON_PERSISTENT, STEP_USE_LIMIT
		};
		queue.clear();
		for (Step s : order) {
			handled[s] = true;
			queue.push_back(s);
		}
		inputCooldown = OPEN_INPUT_COOLDOWN_MS;
		appearElapsed = 0.f;
	}

	void BattleTutorial::Skip()
	{
		queue.clear();
		for (bool& h : handled) h = true;
	}

	void BattleTutorial::EnsureSkipButton(DX9GF::Camera& uiCamera)
	{
		if (skipBtn) return;
		uiTransformManager = std::make_shared<DX9GF::TransformManager>();
		skipBtn = std::make_shared<TextIconButton>(uiTransformManager, 0.f, 0.f,
			static_cast<int>(SKIP_W), static_cast<int>(SKIP_H), uiTex, uiFont, L"Skip>>", 3);
		skipBtn->SetSpriteCoords(16, 0, 32, 16, 0, true);
		skipBtn->SetSpriteScale(SKIP_SCALE, SKIP_SCALE);
		skipBtn->SetTextScale(1.f, 1.f);
		skipBtn->SetTextColor(0xFF000000);
		skipBtn->SetDynamicTextGetter([]() { return Tr(L"Skip >>"); });
		skipBtn->SetOnReleaseLeft([this](DX9GF::ITrigger*) { skipRequested = true; });
		skipBtn->Init(&uiCamera);
		uiTransformManager->RebuildHierarchy();
	}
}
