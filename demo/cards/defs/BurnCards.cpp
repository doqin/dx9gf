#include "pch.h"
#include "BurnCards.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"

void Demo::FanTheFlamesCard::Resolve() {
	if (!battleScene) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead() || !enemy->HasModifier(ModifierType::Burn)) continue;
		// Stacking adds to the value and refreshes the duration to the longer of the two, so passing
		// the current duration leaves it where it is.
		enemy->AddStackingModifier(ModifierType::Burn, enemy->GetModifierDuration(ModifierType::Burn), BURN_ADDED, false);
	}
}

void Demo::FanTheFlamesCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		auto it = state.enemies.find(enemy.get());
		if (it != state.enemies.end() && it->second.burn > 0.f) it->second.burn += BURN_ADDED;
	}
}

void Demo::MeltdownCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy) return;
	const float burn = enemy->GetModifierValue(ModifierType::Burn);
	// Half the block goes first, so the hit that follows meets less of it.
	enemy->RemoveBlock(enemy->GetTemporaryDefense() * 0.5f);
	if (owner) owner->DealDamage(enemy.get(), DamageFor(burn));
}

void Demo::MeltdownCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget();
	if (!enemy) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end()) return;
	it->second.block *= 0.5f;
	state.SimulateDamage(enemy.get(), DamageFor(it->second.burn));
}
