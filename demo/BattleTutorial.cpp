#include "pch.h"
#include "BattleTutorial.h"
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
	}

	BattleTutorial::BattleTutorial(DX9GF::GraphicsDevice* gd)
	{
		sheet = std::make_shared<DX9GF::Texture>(gd);
		sheet->LoadTexture(L"assets/tutorial.png");
		sprite = std::make_shared<DX9GF::StaticSprite>(sheet.get());
		sprite->SetScale(PANEL_SCALE, PANEL_SCALE);
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
		if (queue.empty()) {
			return;
		}
		const float dt = static_cast<float>(deltaTime);
		appearElapsed += dt;
		if (inputCooldown > 0.f) {
			inputCooldown -= dt;
			return;
		}

		auto inp = DX9GF::InputManager::GetInstance();
		const bool dismiss =
			inp->MousePress(DX9GF::InputManager::MouseButton::Left) ||
			inp->KeyPress(DIK_SPACE) ||
			inp->KeyPress(DIK_RETURN);
		if (!dismiss) {
			return;
		}

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
		fontSprite->SetText(L"Click / Space to continue");
		fontSprite->SetColor(D3DCOLOR_ARGB((std::min)(255, hintAlpha), 255, 255, 255));
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		fontSprite->SetPosition(-fontSprite->GetWidth() / 2.f, -18.f + drawnH / 2.f + 18.f);
		fontSprite->Begin();
		fontSprite->Draw(uiCamera, deltaTime);
		fontSprite->End();
		fontSprite->SetOutline(false);
		fontSprite->SetColor(0xFFFFFFFF);

		gd->SetAlphaBlending(false);
	}
}
