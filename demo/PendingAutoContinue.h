#pragma once

namespace Demo {
	// One-shot signal for "load the last save automatically". IBattleScene sets this when the
	// player answers Yes to the defeat popup; MainMenuScene::Init() consumes it to run the same
	// continue flow as clicking the Continue button by hand, so death can skip straight into the
	// loaded save instead of leaving the player to find the button themselves.
	class PendingAutoContinue {
	private:
		PendingAutoContinue() = default;
		bool pending = false;
	public:
		static PendingAutoContinue* GetInstance() {
			static PendingAutoContinue instance;
			return &instance;
		}
		void Request() { pending = true; }
		// Reads and clears in one step so a second Init() (e.g. returning to the main menu later
		// through a different path) never re-triggers it.
		bool ConsumeIfPending() {
			if (!pending) return false;
			pending = false;
			return true;
		}
	};
}