#include "pch.h"
#include "PasswordMenu.h"
#include "LocalizationManager.h"
#include "SettingsManager.h"
#include "TextInputManager.h"
#include <algorithm>
#include <array>

namespace {
	constexpr D3DCOLOR COLOR_CORRECT = 0xFF55FF55;
	constexpr D3DCOLOR COLOR_MISPLACED = 0xFFFFD700;
	constexpr D3DCOLOR COLOR_ABSENT = 0xFF808080;
	constexpr D3DCOLOR COLOR_PENDING = 0xFFFFFFFF;
	constexpr D3DCOLOR PANEL_BG = 0xEE141410;
	constexpr D3DCOLOR PANEL_BORDER = 0xFF55FF55;

	constexpr float PANEL_W = 300.0f;
	constexpr float ROW_H = 26.0f;
	constexpr float DIGIT_SPACING = 34.0f;
	constexpr float CARET_PERIOD = 1000.0f;
	constexpr float HINT_ROW_H = 22.0f;
	constexpr float LEGEND_GAP = 12.0f;
	constexpr float LEGEND_PAD = 10.0f;
	constexpr float LEGEND_SWATCH = 12.0f;
	constexpr float LEGEND_SCALE = 0.8f;
}

namespace Demo {

	void PasswordMenu::Init(DX9GF::Camera* uiCam, std::shared_ptr<DX9GF::Font> font, Game* game)
	{
		this->uiCamera = uiCam;
		this->game = game;
		this->fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	}

	void PasswordMenu::Open(const std::wstring& password)
	{
		this->password = password;
		currentGuess.clear();
		history.clear();
		statusMessage.clear();
		caretTimer = 0.0f;
		isOpen = true;
	}

	void PasswordMenu::Close()
	{
		isOpen = false;
		currentGuess.clear();
	}

	// Two passes so a repeated digit is never over-credited: exact hits claim their digit out of the
	// pool first, and only the leftovers can be scored as misplaced.
	PasswordMenu::ScoredGuess PasswordMenu::Score(const std::wstring& guess) const
	{
		ScoredGuess result;
		result.digits = guess;
		result.states.assign(CODE_LENGTH, DigitState::Absent);

		std::array<int, 10> remaining{};
		remaining.fill(0);

		for (int i = 0; i < CODE_LENGTH; i++) {
			if (guess[i] == password[i]) {
				result.states[i] = DigitState::Correct;
			}
			else {
				remaining[password[i] - L'0']++;
			}
		}

		for (int i = 0; i < CODE_LENGTH; i++) {
			if (result.states[i] == DigitState::Correct) continue;
			int digit = guess[i] - L'0';
			if (remaining[digit] > 0) {
				remaining[digit]--;
				result.states[i] = DigitState::Misplaced;
			}
		}

		return result;
	}

	void PasswordMenu::Submit()
	{
		auto scored = Score(currentGuess);
		bool solved = (currentGuess == password);

		history.push_back(scored);
		currentGuess.clear();

		if (solved) {
			DX9GF::AudioManager::GetInstance()->PlayRandom("power_up", 0.4f);
			if (onSolved) onSolved();
			Close();
			return;
		}

		statusMessage = Tr(L"ACCESS DENIED");
		DX9GF::AudioManager::GetInstance()->PlayRandom("dialog_voice", 0.3f);
		wrongGuesses++;
		UpdateHints();
	}


	void PasswordMenu::UpdateHints()
	{
		size_t wanted = 0;
		for (int threshold : HINT_DIGIT_AFTER) {
			if (wrongGuesses >= threshold) wanted++;
		}

		auto isRevealed = [&](int pos) {
			return std::find(revealedPositions.begin(), revealedPositions.end(), pos) != revealedPositions.end();
			};

		while (revealedPositions.size() < wanted) {
			// Prefer a position the latest guess still has wrong, so the hint is never wasted.
			int pick = -1;
			if (!history.empty()) {
				const auto& last = history.back();
				for (int i = 0; i < CODE_LENGTH && pick < 0; i++) {
					if (last.states[i] != DigitState::Correct && !isRevealed(i)) pick = i;
				}
			}
			for (int i = 0; i < CODE_LENGTH && pick < 0; i++) {
				if (!isRevealed(i)) pick = i;
			}
			if (pick < 0) break;
			revealedPositions.push_back(pick);
		}
	}

	std::vector<std::pair<std::wstring, D3DCOLOR>> PasswordMenu::GetHintLines() const
	{
		std::vector<std::pair<std::wstring, D3DCOLOR>> lines;
		for (int pos : revealedPositions) {
			lines.push_back({ Tr(L"HINT: digit ") + std::to_wstring(pos + 1) + Tr(L" is ") + std::wstring(1, password[pos]),
				COLOR_PENDING });
		}
		return lines;
	}

	void PasswordMenu::Update(unsigned long long deltaTime)
	{
		if (!isOpen) return;

		caretTimer += static_cast<float>(deltaTime);
		if (caretTimer >= CARET_PERIOD) caretTimer -= CARET_PERIOD;

		auto inpMan = DX9GF::InputManager::GetInstance();
		auto settings = SettingsManager::GetInstance();

		// Consume the close key so the same press does not also reach the host scene's inventory toggle.
		int closeKey = settings->GetKeybind("OPEN_INVENTORY");
		if (inpMan->KeyDown(closeKey)) {
			inpMan->ConsumeKey(closeKey);
			Close();
			return;
		}

		for (char c : TextInputManager::GetInstance()->ReadInput()) {
			if (c == '\b') {
				if (!currentGuess.empty()) currentGuess.pop_back();
				statusMessage.clear();
			}
			else if (c >= '0' && c <= '9' && currentGuess.size() < static_cast<size_t>(CODE_LENGTH)) {
				currentGuess.push_back(static_cast<wchar_t>(c));
				statusMessage.clear();
			}
		}

		int acceptKey = settings->GetKeybind("ACCEPT");
		if (currentGuess.size() == static_cast<size_t>(CODE_LENGTH) && inpMan->KeyDown(acceptKey)) {
			inpMan->ConsumeKey(acceptKey);
			Submit();
		}
	}

	void PasswordMenu::Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
	{
		if (!isOpen || !uiCamera || !fontSprite || !game) return;

		float sw = static_cast<float>(game->GetVirtualWidth());
		float sh = static_cast<float>(game->GetVirtualHeight());

		size_t visibleRows = (std::min)(history.size(), MAX_VISIBLE_ROWS);
		size_t firstRow = history.size() - visibleRows;

		const auto hintLines = GetHintLines();

		// title + input row + status + footer, plus one row per visible guess and per hint
		float panelH = 150.0f + static_cast<float>(visibleRows) * ROW_H
			+ static_cast<float>(hintLines.size()) * HINT_ROW_H + (hintLines.empty() ? 0.0f : 8.0f);
		float panelX = -PANEL_W / 2.0f;
		float panelY = -panelH / 2.0f;

		gd->SetAlphaBlending(true);
		gd->DrawRectangle(*uiCamera, -sw / 2.0f, -sh / 2.0f, sw, sh, D3DCOLOR_ARGB(165, 0, 0, 0), true);
		gd->DrawRectangle(*uiCamera, panelX, panelY, PANEL_W, panelH, PANEL_BG, true);
		gd->DrawRectangle(*uiCamera, panelX, panelY, PANEL_W, panelH, PANEL_BORDER, false);
		gd->SetAlphaBlending(false);


		// Colour legend, in its own panel to the right of the code panel: a title, then one row per
		// colour with a swatch. Sized to its longest line.
		const bool showLegend = wrongGuesses >= HINT_LEGEND_AFTER;
		const std::wstring legendText[] = {
			Tr(L"COLOURS"),
			Tr(L"GREEN: right place"),
			Tr(L"YELLOW: wrong place"),
			Tr(L"GREY: not in the code"),
		};
		const D3DCOLOR legendColor[] = { COLOR_PENDING, COLOR_CORRECT, COLOR_MISPLACED, COLOR_ABSENT };
		const float legendX = panelX + PANEL_W + LEGEND_GAP;
		const float legendY = panelY;
		float legendW = 0.0f;
		for (const auto& text : legendText) {
			fontSprite->SetText(text);
			legendW = (std::max)(legendW, static_cast<float>(fontSprite->GetWidth()) * LEGEND_SCALE);
		}
		legendW += LEGEND_PAD * 2.0f + LEGEND_SWATCH + 8.0f;
		const float legendH = LEGEND_PAD * 2.0f + HINT_ROW_H * 4.0f;
		if (showLegend) {
			gd->SetAlphaBlending(true);
			gd->DrawRectangle(*uiCamera, legendX, legendY, legendW, legendH, PANEL_BG, true);
			gd->DrawRectangle(*uiCamera, legendX, legendY, legendW, legendH, PANEL_BORDER, false);
			for (int i = 1; i < 4; i++) {
				const float rowY = legendY + LEGEND_PAD + HINT_ROW_H * i;
				gd->DrawRectangle(*uiCamera, legendX + LEGEND_PAD, rowY + (HINT_ROW_H - LEGEND_SWATCH) / 2.0f - 2.0f,
					LEGEND_SWATCH, LEGEND_SWATCH, legendColor[i], true);
			}
			gd->SetAlphaBlending(false);
		}

		fontSprite->Begin();
		fontSprite->SetOutline(true, 0xFF000000, 2.f);

		auto drawCentered = [&](const std::wstring& text, float y, D3DCOLOR color, float scale) {
			fontSprite->SetScale(scale, scale);
			fontSprite->SetColor(color);
			fontSprite->SetText(text);
			fontSprite->SetPosition(-fontSprite->GetWidth() * scale / 2.0f, y);
			fontSprite->Draw(*uiCamera, deltaTime);
			};

		float y = panelY + 12.0f;
		drawCentered(Tr(L"ENTER 4-DIGIT CODE"), y, COLOR_PENDING, 1.0f);
		y += 30.0f;

		// Guess history, oldest visible first.
		for (size_t r = firstRow; r < history.size(); r++) {
			const auto& row = history[r];
			float startX = -(CODE_LENGTH - 1) * DIGIT_SPACING / 2.0f;
			fontSprite->SetScale(1.2f, 1.2f);
			for (int i = 0; i < CODE_LENGTH; i++) {
				D3DCOLOR color = COLOR_ABSENT;
				if (row.states[i] == DigitState::Correct) color = COLOR_CORRECT;
				else if (row.states[i] == DigitState::Misplaced) color = COLOR_MISPLACED;

				fontSprite->SetColor(color);
				fontSprite->SetText(std::wstring(1, row.digits[i]));
				fontSprite->SetPosition(startX + i * DIGIT_SPACING - fontSprite->GetWidth() * 1.2f / 2.0f, y);
				fontSprite->Draw(*uiCamera, deltaTime);
			}
			y += ROW_H;
		}

		if (visibleRows > 0) y += 8.0f;

		// Current input: filled digits, underscores for the rest, caret on the next empty slot.
		{
			float startX = -(CODE_LENGTH - 1) * DIGIT_SPACING / 2.0f;
			bool caretOn = caretTimer < CARET_PERIOD / 2.0f;
			fontSprite->SetScale(1.4f, 1.4f);
			for (int i = 0; i < CODE_LENGTH; i++) {
				std::wstring glyph;
				if (i < static_cast<int>(currentGuess.size())) glyph = std::wstring(1, currentGuess[i]);
				else if (i == static_cast<int>(currentGuess.size()) && caretOn) glyph = L"|";
				else glyph = L"_";

				fontSprite->SetColor(COLOR_PENDING);
				fontSprite->SetText(glyph);
				fontSprite->SetPosition(startX + i * DIGIT_SPACING - fontSprite->GetWidth() * 1.4f / 2.0f, y);
				fontSprite->Draw(*uiCamera, deltaTime);
			}
			y += 34.0f;
		}

		if (!statusMessage.empty()) {
			drawCentered(statusMessage, y, 0xFFFF5555, 1.0f);
		}
		y += 24.0f;


		// Hints, between the status line and the footer.
		for (const auto& [text, color] : hintLines) {
			drawCentered(text, y, color, 0.8f);
			y += HINT_ROW_H;
		}
		if (!hintLines.empty()) y += 8.0f;

		auto settings = SettingsManager::GetInstance();
		std::wstring footer = settings->GetKeybindDisplayName("ACCEPT") + L" submit   "
			+ settings->GetKeybindDisplayName("OPEN_INVENTORY") + L" exit";
		drawCentered(footer, y, COLOR_ABSENT, 0.8f);


		if (showLegend) {
			fontSprite->SetScale(LEGEND_SCALE, LEGEND_SCALE);
			for (int i = 0; i < 4; i++) {
				// The title starts at the panel's edge; colour rows start after their swatch.
				const float textX = legendX + LEGEND_PAD + (i == 0 ? 0.0f : LEGEND_SWATCH + 8.0f);
				fontSprite->SetColor(legendColor[i]);
				fontSprite->SetText(legendText[i]);
				fontSprite->SetPosition(textX, legendY + LEGEND_PAD + HINT_ROW_H * i);
				fontSprite->Draw(*uiCamera, deltaTime);
			}
		}

		fontSprite->SetOutline(false);
		fontSprite->End();
	}
}
