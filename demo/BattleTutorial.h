#pragma once
#include "DX9GF.h"
#include <memory>
#include <deque>
#include "TextIconButton.h" 

namespace Demo {
	// Snapshot of battle state the tutorial inspects each frame to decide which contextual
	// panel is due next. Built by IBattleScene::Update and handed to Observe().
	struct BattleTutorialContext {
		bool inProgrammingPhase = false; // State::PlayerAttack (dragging cards / making enemy cards)
		int  currentTurn = 1;
		bool cardInBlock = false;            // >= 1 statement card queued in the main or init block
		bool enemyCardExists = false;        // an EnemyCard has been spawned this turn
		bool enemyCardTargeted = false;      // an EnemyCard is attached to a statement card
		bool nonPersistentCardInHand = false;
		bool limitedUseCardInHand = false;
	};

	// The first-battle walkthrough. Nine pre-composed panels live stacked in assets/tutorial-notext.png;
	// this surfaces them one at a time, each at the moment its mechanic first becomes relevant,
	// and swallows battle input while one is on screen (a click / Space / Enter advances).
	class BattleTutorial {
	public:
		BattleTutorial(DX9GF::GraphicsDevice* gd, DX9GF::Font* font);

		// Re-evaluates the triggers and queues any newly-due panel. No-op once every panel
		// has been shown.
		void Observe(const BattleTutorialContext& ctx);

		// True while a panel is on screen and battle input should be held.
		bool IsPanelVisible() const { return !queue.empty(); }

		// Consumes a click / key press to dismiss the visible panel. Call only while a panel
		// is visible.
		void Update(unsigned long long deltaTime);

		// Full-screen dim + the current panel art (3x) + a pulsing "continue" hint.
		void Draw(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera,
			DX9GF::FontSprite* fontSprite, float screenW, float screenH,
			unsigned long long deltaTime);

		// Queue all nine panels in teaching order (used by the Guide button). Ignores what was already shown.
		void ReplayAll();
		// Drop every pending panel and mark all of them as handled so none reappears.
		void Skip();
	private:
		enum Step {
			STEP_DRAG_CARDS = 0, // panel 1 - drag cards into the blocks
			STEP_BLOCKS,         // panel 5 - init vs main block
			STEP_ENERGY,         // panel 7 - energy budget
			STEP_NON_PERSISTENT, // panel 8 - non-persistent cards
			STEP_USE_LIMIT,      // panel 9 - limited-use cards
			STEP_ENEMY_CARD,     // panel 2 - click an enemy for an Enemy Card
			STEP_TARGET,         // panel 3 - drag the Enemy Card onto your card
			STEP_EXECUTE,        // panel 4 - press Execute
			STEP_CYCLE,          // panel 6 - the main block's 3-turn cycle
			STEP_COUNT
		};
		// Source row of each panel in assets/tutorial-notext.png (the sheet is 192px wide). The
		// first `crop` rows are the (now blank) caption band; the caption is drawn with the
		// FontSprite so it can be translated. A panel may also carry a small inline label,
		// drawn at (labelX, labelY) in sheet pixels relative to the panel's top row.
		struct PanelRect { long top; long bottom; long crop; long labelX; long labelY; };
		struct PanelText { const wchar_t* body; const wchar_t* label; };
		static const PanelText TEXTS[STEP_COUNT];
		static const PanelRect PANELS[STEP_COUNT];

		void TryQueue(Step s, bool condition);

		void EnsureSkipButton(DX9GF::Camera& uiCamera);

		DX9GF::GraphicsDevice* device = nullptr;
		std::shared_ptr<DX9GF::Texture> uiTex;
		std::shared_ptr<DX9GF::TransformManager> uiTransformManager;
		DX9GF::Font* uiFont = nullptr;
		std::shared_ptr<TextIconButton> skipBtn;
		bool skipRequested = false;

		std::shared_ptr<DX9GF::Texture> sheet;
		std::shared_ptr<DX9GF::StaticSprite> sprite;
		bool handled[STEP_COUNT] = { false }; // shown, or currently queued
		std::deque<Step> queue;               // pending panels, front() = the visible one
		float inputCooldown = 0.f;            // debounces the action that opened the panel
		float appearElapsed = 0.f;            // drives the "continue" hint pulse
	};
}
