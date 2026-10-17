#include "pch.h"
#include "PoisonCards.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"

void Demo::FesteringCard::Resolve() {
	if (!battleScene) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead() || !enemy->HasModifier(ModifierType::Poison)) continue;
		enemy->AddModifier(ModifierType::Poison, 2, 0.f, false);
	}
}

void Demo::FesteringCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (const auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		auto it = state.enemies.find(enemy.get());
		if (it == state.enemies.end() || it->second.poisonDuration <= 0) continue;
		state.SimulateEnemyModifier(enemy.get(), ModifierType::Poison, 0.f, 2);
	}
}

void Demo::ContagionCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !battleScene) return;
	const int turns = enemy->GetModifierDuration(ModifierType::Poison);
	const float value = enemy->GetModifierValue(ModifierType::Poison);
	if (turns <= 0) return;
	for (const auto& other : battleScene->GetEnemies()) {
		if (!other || other.get() == enemy.get() || other->IsDead()) continue;
		other->AddModifier(ModifierType::Poison, turns, value, false);
	}
}

void Demo::ContagionCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget();
	if (!enemy || !battleScene) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end() || it->second.poisonDuration <= 0) return;

	const int turns = it->second.poisonDuration;
	const float value = it->second.poisonValue;
	for (const auto& other : battleScene->GetEnemies()) {
		if (!other || other.get() == enemy.get() || other->IsDead()) continue;
		state.SimulateEnemyModifier(other.get(), ModifierType::Poison, value, turns);
	}
}

void Demo::SepticStrikeCard::Resolve() {
	auto enemy = LiveTarget();
	if (enemy && owner) {
		owner->DealDamage(enemy.get(), DamageFor(enemy->GetModifierDuration(ModifierType::Poison)));
	}
}

void Demo::SepticStrikeCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget();
	if (!enemy) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end()) return;
	state.SimulateDamage(enemy.get(), DamageFor(it->second.poisonDuration));
}
