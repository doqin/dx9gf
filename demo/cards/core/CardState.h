#pragma once
#include <optional>

namespace Demo {
	// Per-card state that changes during play: lock/retain flags, limited uses and persistence.
	class CardState {
	protected:
		bool isLocked = false;
		int lockedTurns = 0;
		bool isRetained = false;
		// Cards without a cap never deplete. Once a capped card runs out it moves to the battle's
		// nullified pile and never returns to the deck for the rest of the battle.
		std::optional<int> maxUses;
		int usesLeft = 0;
		bool isPersistent = true;
	public:
		bool IsLocked() const { return isLocked; }
		int GetLockedTurns() const { return lockedTurns; }
		void SetLocked(int turns) {
			isLocked = true;
			lockedTurns = turns;
		}
		void TickLock() {
			if (isLocked) {
				lockedTurns--;
				if (lockedTurns <= 0) {
					isLocked = false;
					lockedTurns = 0;
				}
			}
		}

		bool IsRetained() const { return isRetained; }
		void SetRetained(bool retain) { isRetained = retain; }

		bool HasLimitedUses() const { return maxUses.has_value(); }
		int GetMaxUses() const { return maxUses.value_or(0); }
		int GetRemainingUses() const { return maxUses.has_value() ? usesLeft : 0; }
		void SetMaxUses(int uses) {
			maxUses = uses;
			usesLeft = uses;
		}
		void ClearMaxUses() {
			maxUses.reset();
			usesLeft = 0;
		}
		void ConsumeUse() {
			if (maxUses.has_value() && usesLeft > 0) {
				--usesLeft;
			}
		}
		bool IsDepleted() const { return maxUses.has_value() && usesLeft <= 0; }
		// Restores the use counter so the card is playable again in the next battle.
		void ResetUses() {
			if (maxUses.has_value()) {
				usesLeft = maxUses.value();
			}
		}

		bool IsPersistent() const { return isPersistent; }
		void SetPersistent(bool persistent) { isPersistent = persistent; }
	};
}
