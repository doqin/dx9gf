#include "pch.h"
#include "SynergyCards.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"
#include <algorithm>

namespace {
	// The first target's enemy, or null when there is none or it is already dead.
	std::shared_ptr<Demo::IEnemy> LiveTarget(const std::vector<std::weak_ptr<Demo::EnemyCard>>& targets) {
		if (targets.empty()) return nullptr;
		auto card = targets[0].lock();
		if (!card || !card->GetValue() || card->GetValue()->IsDead()) return nullptr;
		return card->GetValue();
	}
}

// ---- Poison --------------------------------------------------------------------------------

bool Demo::ToxicCloudCard::Execute() {
	if (isDone) return true;
	if (battleScene) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead()) continue;
			enemy->AddModifier(ModifierType::Poison, 4, 0.f, false);
		}
	}
	isDone = true;
	return true;
}

void Demo::ToxicCloudCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		state.SimulateEnemyModifier(enemy.get(), ModifierType::Poison, 0.f, 4);
	}
}

bool Demo::FesteringCard::Execute() {
	if (isDone) return true;
	if (battleScene) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead() || !enemy->HasModifier(ModifierType::Poison)) continue;
			enemy->AddModifier(ModifierType::Poison, 2, 0.f, false);
		}
	}
	isDone = true;
	return true;
}

void Demo::FesteringCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		auto it = state.enemies.find(enemy.get());
		if (it == state.enemies.end() || it->second.poisonDuration <= 0) continue;
		state.SimulateEnemyModifier(enemy.get(), ModifierType::Poison, 0.f, 2);
	}
}

bool Demo::ContagionCard::Execute() {
	if (isDone) return true;
	auto enemy = LiveTarget(targets);
	if (enemy && battleScene) {
		const int turns = enemy->GetModifierDuration(ModifierType::Poison);
		const float value = enemy->GetModifierValue(ModifierType::Poison);
		if (turns > 0) {
			for (auto& other : battleScene->GetEnemies()) {
				if (!other || other.get() == enemy.get() || other->IsDead()) continue;
				other->AddModifier(ModifierType::Poison, turns, value, false);
			}
		}
	}
	isDone = true;
	return true;
}

void Demo::ContagionCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget(targets);
	if (!enemy || !battleScene) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end() || it->second.poisonDuration <= 0) return;

	const int turns = it->second.poisonDuration;
	const float value = it->second.poisonValue;
	for (auto& other : battleScene->GetEnemies()) {
		if (!other || other.get() == enemy.get() || other->IsDead()) continue;
		state.SimulateEnemyModifier(other.get(), ModifierType::Poison, value, turns);
	}
}

bool Demo::SepticStrikeCard::Execute() {
	if (isDone) return true;
	if (auto enemy = LiveTarget(targets)) {
		if (owner) owner->DealDamage(enemy.get(), DamageFor(enemy->GetModifierDuration(ModifierType::Poison)));
	}
	isDone = true;
	return true;
}

void Demo::SepticStrikeCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget(targets);
	if (!enemy) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end()) return;
	state.SimulateDamage(enemy.get(), DamageFor(it->second.poisonDuration));
}

// ---- Burn ----------------------------------------------------------------------------------

bool Demo::KindleCard::Execute() {
	if (isDone) return true;
	if (auto enemy = LiveTarget(targets)) {
		enemy->AddStackingModifier(ModifierType::Burn, 3, 4.f, false);
	}
	isDone = true;
	return true;
}

void Demo::KindleCard::CollectProjectedSteps(VirtualBattleState& state) {
	CollectEffectOnTargets(state, ModifierType::Burn, 4.f, 3, 1);
}

bool Demo::FanTheFlamesCard::Execute() {
	if (isDone) return true;
	if (battleScene) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead()) continue;
			if (!enemy->HasModifier(ModifierType::Burn)) continue;
			// Stacking adds to the value and refreshes the duration to the longer of the two, so passing
			// the current duration leaves it where it is.
			enemy->AddStackingModifier(ModifierType::Burn, enemy->GetModifierDuration(ModifierType::Burn), BURN_ADDED, false);
		}
	}
	isDone = true;
	return true;
}

void Demo::FanTheFlamesCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		auto it = state.enemies.find(enemy.get());
		if (it != state.enemies.end() && it->second.burn > 0.f) it->second.burn += BURN_ADDED;
	}
}

bool Demo::MeltdownCard::Execute() {
	if (isDone) return true;
	if (auto enemy = LiveTarget(targets)) {
		const float burn = enemy->GetModifierValue(ModifierType::Burn);
		// Half the block goes first, so the hit that follows meets less of it.
		enemy->RemoveBlock(enemy->GetTemporaryDefense() * 0.5f);
		if (owner) owner->DealDamage(enemy.get(), DamageFor(burn));
	}
	isDone = true;
	return true;
}

void Demo::MeltdownCard::CollectProjectedSteps(VirtualBattleState& state) {
	auto enemy = LiveTarget(targets);
	if (!enemy) return;
	auto it = state.enemies.find(enemy.get());
	if (it == state.enemies.end()) return;
	it->second.block *= 0.5f;
	state.SimulateDamage(enemy.get(), DamageFor(it->second.burn));
}

// ---- Marked --------------------------------------------------------------------------------

bool Demo::BarrageCard::Execute() {
	if (isDone) return true;
	auto enemy = LiveTarget(targets);
	if (!enemy) {
		isDone = true;
		return true;
	}
	// One hit per call, like TwinStrike: the program keeps calling until it reports done, so the
	// hits land one after another instead of as a single lump.
	if (owner) owner->DealDamage(enemy.get(), HIT_DAMAGE);
	++hits;
	if (hits >= HIT_COUNT) {
		isDone = true;
		return true;
	}
	return false;
}

void Demo::BarrageCard::CollectProjectedSteps(VirtualBattleState& state) {
	for (int i = 0; i < HIT_COUNT; ++i) {
		CollectHitsOnTargets(state, HIT_DAMAGE, 1);
	}
}

void Demo::BarrageCard::ResetExecution() {
	MultiTargetCard::ResetExecution();
	hits = 0;
}

bool Demo::DragnetCard::Execute() {
	if (isDone) return true;
	if (battleScene) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead()) continue;
			enemy->AddModifier(ModifierType::Marked, MARKED_TURNS, MARKED_VALUE, false);
		}
	}
	isDone = true;
	return true;
}

void Demo::DragnetCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		state.SimulateEnemyModifier(enemy.get(), ModifierType::Marked, MARKED_VALUE, MARKED_TURNS);
	}
}

bool Demo::HunterCard::Execute() {
	if (isDone) return true;
	if (auto enemy = LiveTarget(targets)) {
		enemy->AddModifier(ModifierType::Marked, 2, 6.f, false);
	}
	isDone = true;
	return true;
}

void Demo::HunterCard::CollectProjectedSteps(VirtualBattleState& state) {
	CollectEffectOnTargets(state, ModifierType::Marked, 6.f, 2, 1);
}
