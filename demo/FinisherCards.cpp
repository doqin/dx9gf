#include "pch.h"
#include "FinisherCards.h"
#include "IBattleScene.h"
#include "LocalizationManager.h"
#include "VirtualBattleState.h"
// Faces live in the rows added below y = 336 in assets/ui.png, as in UtilityCards.cpp.

bool Demo::TerminateCard::Execute() {
	if (isDone) return true;
	if (!targets.empty()) {
		if (auto enemyCard = targets[0].lock()) {
			if (auto enemy = enemyCard->GetValue()) {
				const float maxHealth = enemy->GetMaxHealth();
				const bool finishable = maxHealth > 0.f && (enemy->GetHealth() / maxHealth) < 0.4f;
				if (owner) owner->DealDamage(enemy.get(), finishable ? 60.f : 30.f);
			}
		}
	}
	isDone = true;
	return true;
}

void Demo::TerminateCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (targets.empty()) return;
	if (auto enemyCard = targets[0].lock()) {
		if (auto enemy = enemyCard->GetValue()) {
			float currentVirtualHP = state.enemies[enemy.get()].health;
			const float maxHealth = enemy->GetMaxHealth();
			const bool finishable = maxHealth > 0.f && (currentVirtualHP / maxHealth) < 0.4f;
			
			state.SimulateDamage(enemy.get(), finishable ? 60.f : 30.f);
		}
	}
}

bool Demo::InfernoCard::Execute() {
	if (isDone) return true;
	if (battleScene && owner) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead()) continue;
			owner->DealDamage(enemy.get(), 8.f);
			// Skip the burn if that killed it - a dead enemy lingers in the list until
			// CollectDeadEnemies runs, and there is no point ticking damage on it.
			if (!enemy->IsDead()) {
				enemy->AddStackingModifier(ModifierType::Burn, 3, 5.f, false);
			}
		}
	}
	isDone = true;
	return true;
}

void Demo::InfernoCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;

		state.SimulateDamage(enemy.get(), 8.f);

		if (!state.enemies[enemy.get()].IsDead()) {
			state.SimulateEnemyModifier(enemy.get(), ModifierType::Burn, 5.f, 3);
		}
	}
}

void Demo::InfernoCard::ResetExecution() {
	isDone = false;
}

bool Demo::SystemPurgeCard::Execute() {
	if (isDone) return true;
	if (battleScene && owner) {
		for (auto& enemy : battleScene->GetEnemies()) {
			if (!enemy || enemy->IsDead()) continue;
			owner->DealDamage(enemy.get(), 16.f);
			if (!enemy->IsDead()) {
				enemy->AddModifier(ModifierType::Stun, 1, 0.f, false);
			}
		}
		battleScene->QueuePopUpMessage(Tr(L"System purged"));
	}
	isDone = true;
	return true;
}

void Demo::SystemPurgeCard::CollectProjectedSteps(VirtualBattleState& state) {
	if (!battleScene) return;
	for (auto& enemy : battleScene->GetEnemies()) {
		if (!enemy || enemy->IsDead()) continue;
		state.SimulateDamage(enemy.get(), 16.f);
	}
}

void Demo::SystemPurgeCard::ResetExecution() {
	isDone = false;
}

bool Demo::OverdriveCard::Execute() {
	if (isDone) return true;
	if (owner) {
		owner->AddStackingModifier(ModifierType::BuffDamage, turns, attackBuff, true);
		owner->AddStackingModifier(ModifierType::Regen, turns, regenBuff, true);
		if (battleScene) battleScene->QueuePopUpMessage(Tr(L"Overdrive!"));
	}
	isDone = true;
	return true;
}

void Demo::OverdriveCard::ResetExecution() {
	isDone = false;
}

void Demo::OverdriveCard::CollectProjectedSteps(VirtualBattleState& state) {
	state.player.buffDamage += attackBuff;
}