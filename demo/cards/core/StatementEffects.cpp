#include "pch.h"
#include "IStatementCard.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"

namespace {
	// Enemies an effect applies to, in order. Dead enemies are included: a dead enemy lingers in
	// the scene's list until it is collected, so callers decide whether to skip them.
	std::vector<std::shared_ptr<Demo::IEnemy>> EnemiesFor(
		const Demo::CardEffect& effect,
		const std::vector<std::shared_ptr<Demo::IEnemy>>& targets,
		Demo::IBattleScene* scene) {
		std::vector<std::shared_ptr<Demo::IEnemy>> result;
		if (effect.scope == Demo::EffectScope::AllEnemies) {
			if (scene) {
				for (const auto& enemy : scene->GetEnemies()) {
					if (enemy) result.push_back(enemy);
				}
			}
		}
		else if (effect.scope == Demo::EffectScope::Targets) {
			result = targets;
			if (effect.maxTargets > 0 && result.size() > effect.maxTargets) {
				result.resize(effect.maxTargets);
			}
		}
		return result;
	}
}

bool Demo::IStatementCard::Execute() {
	if (isDone) return true;
	Resolve();
	++stepsDone;
	isDone = stepsDone >= spec.repeat || !HasEffectTarget();
	return isDone;
}

void Demo::IStatementCard::ResetExecution() {
	isDone = false;
	stepsDone = 0;
}

void Demo::IStatementCard::Resolve() {
	ApplyEffects();
}

void Demo::IStatementCard::CollectProjectedSteps(VirtualBattleState& state) {
	for (int i = 0; i < spec.repeat; ++i) {
		ProjectEffects(state);
	}
}

void Demo::IStatementCard::ApplyEffects() {
	const auto targets = GetTargetEnemies();
	for (const auto& effect : spec.effects) {
		if (effect.scope == EffectScope::Self) {
			if (!owner || effect.kind != CardEffect::Kind::Modifier) continue;
			if (effect.stacking) owner->AddStackingModifier(effect.modifier, effect.turns, effect.value, true);
			else owner->AddModifier(effect.modifier, effect.turns, effect.value, true);
			continue;
		}
		for (const auto& enemy : EnemiesFor(effect, targets, battleScene)) {
			if (enemy->IsDead()) continue;
			if (effect.kind == CardEffect::Kind::Damage) {
				if (owner) owner->DealDamage(enemy.get(), effect.damage);
			}
			else if (effect.stacking) {
				enemy->AddStackingModifier(effect.modifier, effect.turns, effect.value, false);
			}
			else {
				enemy->AddModifier(effect.modifier, effect.turns, effect.value, false);
			}
		}
	}
}

void Demo::IStatementCard::ProjectEffects(VirtualBattleState& state) {
	const auto targets = GetTargetEnemies();
	for (const auto& effect : spec.effects) {
		if (effect.scope == EffectScope::Self) {
			// Only an attack buff changes how much damage lands on the enemies.
			if (effect.kind == CardEffect::Kind::Modifier && effect.modifier == ModifierType::BuffDamage) {
				state.player.buffDamage += effect.value;
			}
			continue;
		}
		for (const auto& enemy : EnemiesFor(effect, targets, battleScene)) {
			if (effect.scope == EffectScope::AllEnemies && enemy->IsDead()) continue;
			if (effect.kind == CardEffect::Kind::Damage) {
				state.SimulateDamage(enemy.get(), effect.damage);
			}
			else {
				state.SimulateEnemyModifier(enemy.get(), effect.modifier, effect.value, effect.turns);
			}
		}
	}
}
