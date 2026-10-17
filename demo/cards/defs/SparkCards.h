#pragma once
#include "TargetedCard.h"

namespace Demo {
	// Spark is the consume-and-detonate status: Ignite and the cards below build it up, the two
	// detonators cash it in.

	class IgniteCard : public TargetedCard {
	public:
		IgniteCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Yellow,
				.description = L"Apply Spark 3 for 3 turns.",
				.effects = { Effect::ApplyStacking(ModifierType::Spark, 3, 3.f) } }) {}
	};

	// Spark's cheap setup: pays for itself with a little damage, so it is playable without a
	// detonator in hand. Non-persistent - a persistent copy would out-scale Ignite for the same cost.
	class ShortCircuitCard : public TargetedCard {
	public:
		ShortCircuitCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Yellow,
				.description = L"Deal 4 damage and apply Spark 2 for 3 turns.",
				.persistent = false,
				.effects = { Effect::Hit(4.f), Effect::ApplyStacking(ModifierType::Spark, 3, 2.f) } }) {}
	};

	// A persistent engine rather than a one-off: re-fires every turn it stays installed, so it
	// builds Spark across the whole field while the rest of the hand plays around it.
	class StaticChargeCard : public IStatementCard {
	public:
		StaticChargeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 2,
				.face = CardTemplate::Yellow,
				.description = L"Apply Spark 2 for 3 turns to all enemies.",
				.effects = { Effect::ApplyAllStacking(ModifierType::Spark, 3, 2.f) } }) {}
	};

	class FireDetonationCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 4.f;
		static constexpr float DAMAGE_PER_SPARK = 8.f;
	public:
		FireDetonationCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Yellow,
				.description = L"Deal 4 damage. Consumes all Spark on target to deal 8 extra damage per stack.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// The area payoff. Fire Detonation cashes in one enemy; this cashes in all of them at once and
	// hits the whole field for a little even with no Spark, so it is the card StaticCharge and Ignite
	// are building towards. Non-persistent - it consumes what it detonates, and a persistent one
	// would re-fire every turn off StaticCharge's refill.
	class ChainDetonationCard : public IStatementCard {
	private:
		static constexpr float BASE_DAMAGE = 6.f;
		static constexpr float DAMAGE_PER_SPARK = 8.f;
	public:
		ChainDetonationCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 3,
				.face = CardTemplate::Yellow,
				.description = L"Deal 6 damage to all enemies, plus 8 per Spark stack on each. Consumes all Spark.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};
}
