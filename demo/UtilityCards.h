#pragma once
#include "IStatementCard.h"
#include "MultiTargetCard.h"
#include "LocalizationManager.h"

namespace Demo {

	// The 0-cost tier. Every card here is non-persistent, and that is what pays for the zero
	// cost: a card left in the program re-executes free on every turn of the cycle, so a 0-cost
	// persistent card would be unlimited free value. These fire once per placement and have to
	// be drawn and placed again to fire again.

	class JabCard : public MultiTargetCard {
	public:
		JabCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Jab", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 0; }
		std::wstring GetDescription() const override { return Tr(L"Deal 3 damage to an enemy."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectHitsOnTargets(state, 3.f, 1);
		}
	};

	class MarkCard : public MultiTargetCard {
	public:
		MarkCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Mark", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 0; }
		std::wstring GetDescription() const override { return Tr(L"Apply Marked 4 for 2 turns (target takes 4 extra damage per hit)."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Marked, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Periwinkle; }

		bool Execute() override;
		// Marked takes the strongest source, so a second Mark ahead of a hit adds nothing to the readout.
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Marked, 4.f, 2, 1);
		}
	};

	class BraceCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		BraceCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 0; }
		std::wstring GetDescription() const override { return Tr(L"Gain 5 block for 2 turns."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::BuffDefense, true } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Steel; }

		bool Execute() override;
		void ResetExecution() override;
	};

	// Instant draw. In the init block the new cards arrive in time to be played this turn; in
	// the main block they arrive as the turn ends and are discarded with the rest of the hand.
	class PrefetchCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		PrefetchCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
			SetPersistent(false);
			SetMaxUses(2);
		}

		size_t GetCost() const override { return 0; }
		std::wstring GetDescription() const override { return Tr(L"Draw 2 cards now."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Blue; }

		bool Execute() override;
		void ResetExecution() override;
	};

	class OverclockCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		OverclockCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 192, 32, x, y) {
			SetPersistent(false);
			SetMaxUses(2);
		}

		size_t GetCost() const override { return 0; }
		std::wstring GetDescription() const override { return Tr(L"Gain 1 energy now. Take 4 damage."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Cyan; }

		bool Execute() override;
		void ResetExecution() override;
	};

	// The 1-cost pair that contrasts the two flavours of resource. Jumpstart pays out now and is
	// only worth playing from the init block; Foresight pays out next turn and, being persistent,
	// keeps paying out every turn until the program clears.

	class JumpstartCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		JumpstartCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Gain 2 energy now."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Cyan; }

		bool Execute() override;
		void ResetExecution() override;
	};

	class ForesightCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		ForesightCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Draw 1 extra card next turn."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Blue; }

		bool Execute() override;
		void ResetExecution() override;
	};

	// A legendary utility card that clears all debuffs from the player.
	// Costs 1 energy, max 1 use per battle, non-persistent.
	class SystemRestartCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		SystemRestartCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 192, 32, x, y) {
			SetPersistent(false);
			SetMaxUses(1);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Clear all debuffs and tick damage from yourself."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Sage; }

		bool Execute() override;
		void ResetExecution() override;
	};
}
