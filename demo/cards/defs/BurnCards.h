#pragma once
#include "TargetedCard.h"
#include <algorithm>

namespace Demo {
	// Build-around cards for Burn: a steady flat tick that ignores block. These amplify it and use it
	// against armor.

	// A cheap way into Burn, which otherwise only comes from the cost-3 Inferno.
	class KindleCard : public TargetedCard {
	public:
		KindleCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Orange,
				.description = L"Apply Burn 4 for 3 turns.",
				.persistent = false,
				.effects = { Effect::ApplyStacking(ModifierType::Burn, 3, 4.f) } }) {}
	};

	// Hits every living enemy with no EnemyCard attached, and stacks its Burn each turn it
	// re-fires, so installing it early in a cycle is worth much more than late.
	class InfernoCard : public IStatementCard {
	public:
		InfernoCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 3,
				.face = CardTemplate::Orange,
				.description = L"Deal 8 damage to all enemies and apply Burn 5 for 3 turns.",
				.effects = { Effect::HitAll(8.f), Effect::ApplyAllStacking(ModifierType::Burn, 3, 5.f) } }) {}
	};

	// Adds a flat amount rather than multiplying: Burn stacks and Inferno keeps refilling it, so
	// doubling it compounded into triple-digit ticks within a few plays.
	class FanTheFlamesCard : public IStatementCard {
	private:
		static constexpr float BURN_ADDED = 3.f;
	public:
		FanTheFlamesCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Orange,
				.description = L"Add 3 Burn to all enemies that are already burning.",
				.persistent = false,
				.extraStatusEffects = { { ModifierType::Burn, true } } }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// Burn ignores block, so this is the answer to armored enemies: it hits for the Burn it finds
	// and breaks half the block along the way, leaving the Burn ticking. Burn stacks fast and runs
	// out fast, so the card pays full rate for the first FULL_RATE_BURN points and a trickle beyond
	// that - a big stack is still rewarded, it just stops compounding into huge hits.
	class MeltdownCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 4.f;
		static constexpr float FULL_RATE = 3.f;
		static constexpr float FULL_RATE_BURN = 10.f;
		static constexpr float TAPERED_RATE = 1.f;
	public:
		MeltdownCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Orange,
				.description = L"Deal 4 damage, plus 3 per point of Burn on the target (1 per point beyond 10), and remove half its block. Burn is not consumed.",
				.persistent = false }) {}

		static float DamageFor(float burn) {
			const float full = (std::min)(burn, FULL_RATE_BURN);
			const float tapered = (std::max)(0.f, burn - FULL_RATE_BURN);
			return BASE_DAMAGE + FULL_RATE * full + TAPERED_RATE * tapered;
		}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};
}
