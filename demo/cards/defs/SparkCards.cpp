#include "pch.h"
#include "SparkCards.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"

void Demo::FireDetonationCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy) return;
	const float sparkStacks = enemy->ConsumeModifier(ModifierType::Spark);
	if (owner) owner->DealDamage(enemy.get(), BASE_DAMAGE + sparkStacks * DAMAGE_PER_SPARK);
}

void Demo::FireDetonationCard::CollectProjectedSteps(VirtualBattleState& state) {
	const auto attached = GetTargetEnemies();
	if (attached.empty()) return;
	IEnemy* enemy = attached[0].get();
	auto it = state.enemies.find(enemy);
	if (it == state.enemies.end()) return;
	const float damage = BASE_DAMAGE + it->second.spark * DAMAGE_PER_SPARK;
	it->second.spark = 0.f;
	state.SimulateDamage(enemy, damage);
}

void Demo::ChainDetonationCard::Resolve() {
	if (!battleScene || !owner) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		const float stacks = enemy->ConsumeModifier(ModifierType::Spark);
		owner->DealDamage(enemy.get(), BASE_DAMAGE + stacks * DAMAGE_PER_SPARK);
	}
}

void Demo::ChainDetonationCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		auto it = state.enemies.find(enemy.get());
		if (it == state.enemies.end()) continue;
		const float stacks = it->second.spark;
		it->second.spark = 0.f;
		state.SimulateDamage(enemy.get(), BASE_DAMAGE + stacks * DAMAGE_PER_SPARK);
	}
}
