#include "pch.h"
#include "BattleTutorial.h"
#include "LocalizationManager.h"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace Demo {

	// Panel rows in assets/tutorial-notext.png (192px wide sheet). Order matches the Step enum.
	// { top, bottom, crop (blank caption rows), labelX, labelY (-1 = no inline label) }
	const BattleTutorial::PanelRect BattleTutorial::PANELS[BattleTutorial::STEP_COUNT] = {
		{ 242, 322, 12, 0, -1 },   // STEP_DRAG_CARDS
		{ 558, 674, 48, 0, -1 },   // STEP_BLOCKS
		{ 799, 871, 42, 32, 15 },  // STEP_ENERGY
		{ 878, 954, 41, 0, -1 },   // STEP_NON_PERSISTENT
		{ 984, 1048, 32, 0, -1 },  // STEP_USE_LIMIT
		{ 322, 401, 12, 0, -1 },   // STEP_ENEMY_CARD
		{ 401, 498, 28, 0, -1 },   // STEP_TARGET
		{ 498, 553, 26, 0, -1 },   // STEP_EXECUTE
		{ 700, 786, 32, 31, 12 },  // STEP_CYCLE
		{ 0, 0, 0, 0, -1 },        // STEP_DODGE (drawn from tutorial-dodge.png)
	};

	const BattleTutorial::PanelText BattleTutorial::TEXTS[BattleTutorial::STEP_COUNT] = {
		{ L"Move cards to the playing pile by dragging", nullptr },
		{ L"These are program blocks, where you drag your cards into. Use the Init block when you want to play a card without ending a turn, via the INIT button. Otherwise, you should use the main block for cards to persist", nullptr },
		{ L"You get a set amount of energy each turn. The default amount is 3. Each card costs energy, use your budget wisely.", L"3/3" },
		{ L"Some cards are Non-persistent. Which means they won't stay in the main block after it's played even if the cycle hasn't ended", nullptr },
		{ L"Some cards have a use limit. Which means it's unusable after being played a number of times", nullptr },
		{ L"Click on the enemy to get an Enemy Card", nullptr },
		{ L"Move the enemy card to your card to target the enemy", nullptr },
		{ L"Click the Execute button to initiate your attack", nullptr },
		{ L"The main block has a cycle, which is 3 turns. After a cycle, the cards inside the block is cleared to a blank slate", L"3 turns left before program clears" },
		{ L"During the enemy's attack, move around with W A S D to dodge the projectiles", nullptr },
	};

	namespace {
		constexpr float PANEL_SHEET_WIDTH = 192.f;
		constexpr float PANEL_SCALE = 3.f;
		constexpr float OPEN_INPUT_COOLDOWN_MS = 200.f;
		constexpr float CAPTION_GAP = 12.f;
		constexpr float CAPTION_LINE_SPACING = 4.f;

		// Greedy word-wrap of `text` (honouring newlines) to `maxWidth` using the sprite's metrics.
		std::vector<std::wstring> WrapText(DX9GF::FontSprite* fs, const std::wstring& text, float maxWidth)
		{
			std::vector<std::wstring> lines;
			size_t pos = 0;
			while (pos <= text.size()) {
				size_t nl = text.find(L'\n', pos);
				std::wstring para = text.substr(pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
				std::wstring line;
				size_t p = 0;
				while (p < para.size()) {
					size_t sp = para.find(L' ', p);
					std::wstring word = para.substr(p, sp == std::wstring::npos ? std::wstring::npos : sp - p);
					std::wstring cand = line.empty() ? word : line + L" " + word;
					fs->SetText(cand);
					if (!line.empty() && fs->GetWidth() > maxWidth) {
						lines.push_back(line);
						line = word;
					}
					else {
						line = cand;
					}
					if (sp == std::wstring::npos) break;
					p = sp + 1;
				}
				lines.push_back(line);
				if (nl == std::wstring::npos) break;
				pos = nl + 1;
			}
			return lines;
		}

		constexpr float SKIP_SCALE = 3.f;
		constexpr float SKIP_W = 32.f * SKIP_SCALE;
		constexpr float SKIP_H = 16.f * SKIP_SCALE;
		constexpr float SKIP_MARGIN = 24.f;
	}

	BattleTutorial::BattleTutorial(DX9GF::GraphicsDevice* gd, DX9GF::Font* font)
		: device(gd), uiFont(font)
	{
		sheet = std::make_shared<DX9GF::Texture>(gd);
		sheet->LoadTexture(L"assets/tutorial-notext.png");
		sprite = std::make_shared<DX9GF::StaticSprite>(sheet.get());
		sprite->SetScale(PANEL_SCALE, PANEL_SCALE);

		dodgeTex = std::make_shared<DX9GF::Texture>(gd);
		dodgeTex->LoadTexture(L"assets/tutorial-dodge.png");
		dodgeSprite = std::make_shared<DX9GF::StaticSprite>(dodgeTex.get());

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
		TryQueue(STEP_DODGE, ctx.inEnemyAttackPhase);

		const bool onTurnTwo = ctx.inProgrammingPhase && ctx.currentTurn >= 2;
		TryQueue(STEP_CYCLE, onTurnTwo);
		// Card-lifecycle notes: shown only once such a card turns up in the player's hand.
		TryQueue(STEP_NON_PERSISTENT, ctx.nonPersistentCardInHand);
		TryQueue(STEP_USE_LIMIT, ctx.limitedUseCardInHand);
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
		const PanelText& t = TEXTS[queue.front()];
		const bool dodge = (queue.front() == STEP_DODGE);
		// The dodge panel uses its own (large) image, scaled down to roughly half the screen.
		const float dodgeScale = dodge ? (screenH * 0.5f) / static_cast<float>(dodgeTex->GetHeight()) : 1.f;
		const float gfxH = dodge ? static_cast<float>(dodgeTex->GetHeight()) * dodgeScale
			: static_cast<float>(r.bottom - r.top - r.crop) * PANEL_SCALE;
		const float panelW = dodge ? static_cast<float>(dodgeTex->GetWidth()) * dodgeScale
			: PANEL_SHEET_WIDTH * PANEL_SCALE;
		const float wrapW = (std::max)(panelW, PANEL_SHEET_WIDTH * PANEL_SCALE);

		gd->SetAlphaBlending(true);
		gd->DrawRectangle(uiCamera, -screenW / 2.f, -screenH / 2.f, screenW, screenH,
			D3DCOLOR_ARGB(190, 0, 0, 0), true);

		// Caption (translated), wrapped to the panel width and centred above the artwork.
		fontSprite->SetScale(1.f, 1.f);
		const std::vector<std::wstring> lines = WrapText(fontSprite, Tr(t.body), wrapW);
		const float lineH = static_cast<float>(fontSprite->GetHeight()) + CAPTION_LINE_SPACING;
		const float captionH = lineH * static_cast<float>(lines.size());
		const float totalH = captionH + CAPTION_GAP + gfxH;
		const float top = -18.f - totalH / 2.f;
		const float gfxTop = top + captionH + CAPTION_GAP;

		fontSprite->SetColor(0xFFFFFFFF);
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		for (size_t i = 0; i < lines.size(); ++i) {
			fontSprite->SetText(lines[i]);
			fontSprite->SetPosition(-fontSprite->GetWidth() / 2.f, top + lineH * static_cast<float>(i));
			fontSprite->Begin();
			fontSprite->Draw(uiCamera, deltaTime);
			fontSprite->End();
		}

		if (dodge) {
			dodgeSprite->SetScale(dodgeScale, dodgeScale);
			dodgeSprite->SetOrigin(static_cast<float>(dodgeTex->GetWidth()) / 2.f, 0.f);
			dodgeSprite->SetPosition(0.f, gfxTop);
			dodgeSprite->Begin();
			dodgeSprite->Draw(uiCamera, deltaTime);
			dodgeSprite->End();
		}
		else {
			sprite->SetSrcRect(RECT{ 0, r.top + r.crop, static_cast<LONG>(PANEL_SHEET_WIDTH), r.bottom });
			sprite->SetOrigin(PANEL_SHEET_WIDTH / 2.f, 0.f);
			sprite->SetPosition(0.f, gfxTop);
			sprite->Begin();
			sprite->Draw(uiCamera, deltaTime);
			sprite->End();
		}

		// Inline label next to an icon in the artwork (e.g. the energy / cycle counters).
		if (t.label && r.labelY >= 0) {
			fontSprite->SetText(Tr(t.label));
			fontSprite->SetPosition(-panelW / 2.f + static_cast<float>(r.labelX) * PANEL_SCALE,
				gfxTop + static_cast<float>(r.labelY) * PANEL_SCALE - fontSprite->GetHeight() / 2.f);
			fontSprite->Begin();
			fontSprite->Draw(uiCamera, deltaTime);
			fontSprite->End();
		}

		// Pulsing "click to continue" hint below the panel.
		const int hintAlpha = 150 + static_cast<int>(90.0 * (0.5 + 0.5 * std::sin(appearElapsed * 0.006)));
		fontSprite->SetScale(1.f, 1.f);
		fontSprite->SetText(Tr(L"Click / Space to continue"));
		fontSprite->SetColor(D3DCOLOR_ARGB((std::min)(255, hintAlpha), 255, 255, 255));
		fontSprite->SetOutline(true, 0xFF000000, 2.f);
		fontSprite->SetPosition(-fontSprite->GetWidth() / 2.f, gfxTop + gfxH + 18.f);
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
			STEP_ENEMY_CARD, STEP_TARGET, STEP_EXECUTE, STEP_DODGE, STEP_CYCLE,
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
