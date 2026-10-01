#pragma once
#include "IStatementCard.h"
#include "MultiTargetCard.h"
#include "LocalizationManager.h"
#include <algorithm>

namespace Demo {

	// Build-around cards for Poison, Burn and Marked. Each status keeps its own identity - none of
	// these consume what they read, because consume-and-detonate belongs to Spark:
	//   Poison  is time: its turns are its damage, so these add, copy and read turns.
	//   Burn    is a steady flat tick that ignores block: these amplify it and use it against armor.
	//   Marked  is a flat bonus on every hit: these mark in bulk and reward hitting many times.

	// ---- Poison ----------------------------------------------------------------------------

	// The area source. Non-persistent: re-poisoning every enemy each turn would add turns forever.
	class ToxicCloudCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		ToxicCloudCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 192, 32, x, y) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Apply Poison 4 to all enemies (damage equals remaining turns)."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Poison, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Green; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void ResetExecution() override { isDone = false; }
	};

	// The engine: Poison's damage is its turns left, so adding turns every round makes each tick
	// bigger than the last instead of shrinking.
	class FesteringCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		FesteringCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Add 2 turns of Poison to every poisoned enemy."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Poison, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Green; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void ResetExecution() override { isDone = false; }
	};

	// Copies rather than moves, so the target keeps its own Poison.
	class ContagionCard : public MultiTargetCard {
	public:
		ContagionCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Contagion", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Copy the target's Poison to all other enemies."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Poison, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Green; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// A payoff that leaves the Poison where it is, so it can be played again next turn. Poison turns
	// add up without limit (Festering makes sure of it), so the bonus counts at most POISON_CAP of
	// them: uncapped, a stacked boss turned this into the best single-target card for 1 energy.
	class SepticStrikeCard : public MultiTargetCard {
	private:
		static constexpr float BASE_DAMAGE = 3.f;
		static constexpr float DAMAGE_PER_TURN = 1.f;
		static constexpr int POISON_CAP = 8;
	public:
		SepticStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Septic Strike", x, y, 192, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 3 damage, plus 1 per turn of Poison on the target (up to 8). Poison is not consumed."); }
		static float DamageFor(int poisonTurns) {
			return BASE_DAMAGE + DAMAGE_PER_TURN * static_cast<float>((std::min)(poisonTurns, POISON_CAP));
		}
		CardTemplate GetCardTemplate() const override { return CardTemplate::Green; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// ---- Burn ------------------------------------------------------------------------------

	// A cheap way into Burn, which until now only came from the cost-3 Inferno.
	class KindleCard : public MultiTargetCard {
	public:
		KindleCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Kindle", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Burn 4 for 3 turns."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Burn, true } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Orange; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// Adds a flat amount rather than multiplying: Burn stacks and Inferno keeps refilling it, so
	// doubling it compounded into triple-digit ticks within a few plays.
	class FanTheFlamesCard : public IStatementCard {
	private:
		bool isDone = false;
		static constexpr float BURN_ADDED = 3.f;
	public:
		FanTheFlamesCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 224, 32, x, y) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Add 3 Burn to all enemies that are already burning."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Burn, true } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Orange; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void ResetExecution() override { isDone = false; }
	};

	// Burn ignores block, so this is the answer to armored enemies: it hits for the Burn it finds
	// and breaks half the block along the way, leaving the Burn ticking. Burn stacks fast and runs
	// out fast, so the card pays full rate for the first FULL_RATE_BURN points and a trickle beyond
	// that - a big stack is still rewarded, it just stops compounding into huge hits.
	class MeltdownCard : public MultiTargetCard {
	private:
		static constexpr float BASE_DAMAGE = 4.f;
		static constexpr float FULL_RATE = 3.f;
		static constexpr float FULL_RATE_BURN = 10.f;
		static constexpr float TAPERED_RATE = 1.f;
	public:
		MeltdownCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Meltdown", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override {
			return Tr(L"Deal 4 damage, plus 3 per point of Burn on the target (1 per point beyond 10), and remove half its block. Burn is not consumed.");
		}
		static float DamageFor(float burn) {
			const float full = (std::min)(burn, FULL_RATE_BURN);
			const float tapered = (std::max)(0.f, burn - FULL_RATE_BURN);
			return BASE_DAMAGE + FULL_RATE * full + TAPERED_RATE * tapered;
		}
		CardTemplate GetCardTemplate() const override { return CardTemplate::Orange; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// ---- Marked ----------------------------------------------------------------------------

	// Four hits, so every point of Marked is counted four times.
	class BarrageCard : public MultiTargetCard {
	private:
		int hits = 0;
		static constexpr int HIT_COUNT = 4;
		static constexpr float HIT_DAMAGE = 3.f;
	public:
		BarrageCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Barrage", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 3 damage to an enemy 4 times."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Periwinkle; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void ResetExecution() override;
	};

	// Marked takes the strongest source rather than stacking, so Dragnet has to beat Mark's 4 on a
	// single target - most bosses are one - not just on the field. Twice Mark's strength for 1
	// energy does, and the spread to every other enemy is the bonus on top.
	class DragnetCard : public IStatementCard {
	private:
		bool isDone = false;
		static constexpr float MARKED_VALUE = 8.f;
		static constexpr int MARKED_TURNS = 2;
	public:
		DragnetCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), IStatementCard(tm, 160, 32, x, y) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Marked 8 for 2 turns to all enemies."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Marked, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Periwinkle; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void ResetExecution() override { isDone = false; }
	};

	// Marked no longer stacks, so there is nothing to build; installed early, this keeps the target
	// at a strong Marked for the whole cycle without spending a card per turn on it.
	class HunterCard : public MultiTargetCard {
	public:
		HunterCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Hunter", x, y, 160, 32) {
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Marked 6 for 2 turns to the target each turn."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Marked, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Periwinkle; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};
}
