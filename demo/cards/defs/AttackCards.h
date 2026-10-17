#pragma once
#include "TargetedCard.h"

namespace Demo {
	// Plain damage dealers, from the 0-cost Jab up to the conditional finishers. Cards whose effects
	// are fully described by their CardSpec need no code in AttackCards.cpp.

	class StrikeCard : public TargetedCard {
	public:
		StrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 5 damage to an enemy.",
				.effects = { Effect::Hit(5.f) } }) {}
	};

	// Non-persistent: a persistent copy would re-execute free on every turn of the cycle.
	class JabCard : public TargetedCard {
	public:
		JabCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 0,
				.face = CardTemplate::Red,
				.description = L"Deal 3 damage to an enemy.",
				.persistent = false,
				.effects = { Effect::Hit(3.f) } }) {}
	};

	class HeavyStrikeCard : public TargetedCard {
	public:
		HeavyStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Red,
				.description = L"Deal 16 damage to an enemy.",
				.effects = { Effect::Hit(16.f) } }) {}
	};

	class TwinStrikeCard : public TargetedCard {
	public:
		TwinStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 3 damage to an enemy twice.",
				.effects = { Effect::Hit(3.f) },
				.repeat = 2 }) {}
	};

	class CleaveCard : public TargetedCard {
	public:
		CleaveCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 2, {
				.cost = 2,
				.face = CardTemplate::Red,
				.description = L"Deal 7 damage to up to 2 enemies.",
				.effects = { Effect::Hit(7.f, 0) } }) {}
	};

	// Hits every attached target, with the damage cut each time an enemy is picked again.
	class ChainLightningCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 20.f;
		static constexpr float REDUCTION_PER_REPEAT = 75.f;
		static constexpr size_t MAX_TARGETS = 3;
		std::vector<std::pair<std::shared_ptr<IEnemy>, float>> PlanHits() const;
	public:
		ChainLightningCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, MAX_TARGETS, { .cost = 3, .face = CardTemplate::Red }) {}

		std::wstring GetDescription() const override;
		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// Gains 3 damage every time it is played; OnDiscard resets it.
	class RagingStrikeCard : public TargetedCard {
	private:
		static constexpr int STARTING_DAMAGE = 4;
		static constexpr int DAMAGE_GAIN = 3;
		int currentDamage = STARTING_DAMAGE;
	public:
		RagingStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, { .cost = 1, .face = CardTemplate::Red }) {}

		std::wstring GetDescription() const override;
		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		void OnDiscard() override { currentDamage = STARTING_DAMAGE; }
	};

	// Scales with the persistent cards installed in the same block.
	class OverloadCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 5.f;
		static constexpr float DAMAGE_PER_PERSISTENT = 4.f;
		float TotalDamage() const;
	public:
		OverloadCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Red,
				.description = L"Deal 5 DMG. Deal +4 DMG for each Persistent card currently in the Block.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	// Redirects to the lowest-health enemy when the card before it in the block killed its target.
	class ChainReactionCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 5.f;
		static constexpr float REDIRECTED_DAMAGE = 8.f;
		std::shared_ptr<TargetedCard> PreviousCard() const;
	public:
		ChainReactionCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 5 DMG. If the previous card killed its target, deal 8 DMG to the lowest HP enemy instead." }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	class LethalHarvestCard : public TargetedCard {
	public:
		LethalHarvestCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 5 DMG. If fatal, heal 8 HP." }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 5.f, 1); }
	};

	class ExecuteCard : public TargetedCard {
	public:
		ExecuteCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Red,
				.description = L"Deal 15 DMG. If fatal, gain 1 Energy next turn.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 15.f, 1); }
	};

	class ArmorPiercerCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 5.f;
	public:
		ArmorPiercerCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 5 DMG. Deals 2x damage to enemies with Armor. Ignores their defense." }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	class CruelStrikeCard : public TargetedCard {
	private:
		static constexpr float BASE_DAMAGE = 8.f;
	public:
		CruelStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Deal 8 DMG. Deals 2x damage if target is Weak.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	class ShieldBashCard : public TargetedCard {
	private:
		static constexpr float DAMAGE_PER_ARMOR = 1.5f;
	public:
		ShieldBashCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Red,
				.description = L"Consume all your Armor. Deal damage equal to 1.5x the consumed amount.",
				.persistent = false }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};

	class TerminateCard : public TargetedCard {
	private:
		static constexpr float DAMAGE = 30.f;
		static constexpr float FINISHER_DAMAGE = 60.f;
		static constexpr float FINISHER_HEALTH_RATIO = 0.4f;
	public:
		TerminateCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 3,
				.face = CardTemplate::Red,
				.description = L"Deal 30 damage to an enemy, or 60 if it is below 40% health." }) {}

		void Resolve() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
	};
}
