#pragma once
#include "TargetedCard.h"
#include <algorithm>

namespace Demo {
	// Build-around cards for Poison. Poison is time: its turns are its damage, so these add, copy and
	// read turns, and none of them consume what they read.

	class PoisonCard : public TargetedCard {
	private:
		static constexpr int POISON_TURNS = 3;
	public:
		PoisonCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Green,
				.persistent = false,
				.effects = { Effect::Apply(ModifierType::Poison, POISON_TURNS) } }) {}

		std::wstring GetDescription() const override {
			return Tr(L"Apply Poison ") + std::to_wstring(POISON_TURNS) + Tr(L" (damage equals remaining turns).");
		}
	};

	// The area source. Non-persistent: re-poisoning every enemy each turn would add turns forever.
	class ToxicCloudCard : public IStatementCard {
	public:
		ToxicCloudCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 2,
				.face = CardTemplate::Green,
				.description = L"Apply Poison 4 to all enemies (damage equals remaining turns).",
				.persistent = false,
				.effects = { Effect::ApplyAll(ModifierType::Poison, 4) } }) {}
	};

	// The engine: Poison's damage is its turns left, so adding turns every round makes each tick
	// bigger than the last instead of shrinking.
	class FesteringCard : public IStatementCard {
	public:
		FesteringCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 2,
				.face = CardTemplate::Green,
				.description = L"Add 2 turns of Poison to every poisoned enemy.",
				.extraStatusEffects = { { ModifierType::Poison, false } } }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// Copies rather than moves, so the target keeps its own Poison.
	class ContagionCard : public TargetedCard {
	public:
		ContagionCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Green,
				.description = L"Copy the target's Poison to all other enemies.",
				.persistent = false,
				.extraStatusEffects = { { ModifierType::Poison, false } } }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// A payoff that leaves the Poison where it is, so it can be played again next turn. Poison turns
	// add up without limit (Festering makes sure of it), so the bonus counts at most POISON_CAP of
	// them: uncapped, a stacked boss turned this into the best single-target card for 1 energy.
	class SepticStrikeCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 3.f;
		static constexpr float DAMAGE_PER_TURN = 1.f;
		static constexpr int POISON_CAP = 8;
	public:
		SepticStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Green,
				.description = L"Deal 3 damage, plus 1 per turn of Poison on the target (up to 8). Poison is not consumed.",
				.persistent = false }) {}

		static float DamageFor(int poisonTurns) {
			return BASE_DAMAGE + DAMAGE_PER_TURN * static_cast<float>((std::min)(poisonTurns, POISON_CAP));
		}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};
}
