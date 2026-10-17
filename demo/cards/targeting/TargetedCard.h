#pragma once
#include "IStatementCard.h"
#include "EnemyCard.h"
#include <vector>
#include <string>

namespace Demo {
	// A statement card with slots for enemy cards. Enemy cards are dropped onto it once it sits in
	// a block, and its effects act on whichever are attached. Single-target cards are simply a
	// TargetedCard with one slot.
	class TargetedCard : public IStatementCard {
	protected:
		std::vector<std::weak_ptr<EnemyCard>> targets;
		size_t maxTargets;

		// Enemy in the first slot, or null when the slot is empty or the enemy is already dead.
		std::shared_ptr<IEnemy> LiveTarget() const;
		// Appends one hit of `damage` per attached target, stopping after `maxHits` of them (0
		// means every target), for cards whose damage is worked out in code.
		void CollectHitsOnTargets(VirtualBattleState& state, float damage, size_t maxHits = 0);
		// The same walk for cards that apply a modifier to their targets rather than hitting them.
		void CollectEffectOnTargets(VirtualBattleState& state, ModifierType modifier, float value, int duration, size_t maxTargetsHit = 0);

		bool HasEffectTarget() const override;
	public:
		TargetedCard(std::weak_ptr<DX9GF::TransformManager> tm, size_t maxTargets, CardSpec cardSpec);

		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
		std::wstring GetInputsDescription() const override;
		bool HasRequiredTargets() const override { return !targets.empty(); }
		size_t GetInputSlotCount() const override { return maxTargets; }
		size_t GetFilledInputCount() const override;
		bool CanAcceptEnemyCard() const override { return targets.size() < maxTargets && IsQueuedInBlock(); }
		bool AttachEnemyCard(std::shared_ptr<EnemyCard> card) override;
		void ReleaseEnemyCards() override;
		std::tuple<float, float> GetEnemyCardSlotWorldPosition() const override;
		const std::vector<std::weak_ptr<EnemyCard>>& GetTargets() const { return targets; }
		std::vector<std::shared_ptr<IEnemy>> GetTargetEnemies() const override;
	};
}
