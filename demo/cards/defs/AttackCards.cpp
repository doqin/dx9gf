#include "pch.h"
#include "AttackCards.h"
#include "IBlockCard.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"
#include <limits>

// ---- Chain Lightning -----------------------------------------------------------------------

std::vector<std::pair<std::shared_ptr<Demo::IEnemy>, float>> Demo::ChainLightningCard::PlanHits() const {
	std::vector<std::pair<std::shared_ptr<IEnemy>, float>> hits;
	for (const auto& enemy : GetTargetEnemies()) {
		if (hits.size() >= MAX_TARGETS) break;
		float damage = BASE_DAMAGE;
		for (const auto& earlier : hits) {
			if (earlier.first == enemy) damage *= (100.f - REDUCTION_PER_REPEAT) / 100.f;
		}
		hits.emplace_back(enemy, damage);
	}
	return hits;
}

std::wstring Demo::ChainLightningCard::GetDescription() const {
	return Tr(L"Deal ") + std::to_wstring(static_cast<int>(BASE_DAMAGE)) + Tr(L" damage to up to 3 enemies, but base damage is reduced by ")
		+ std::to_wstring(static_cast<int>(REDUCTION_PER_REPEAT)) + Tr(L"% \neach time it targets the same enemy.");
}

void Demo::ChainLightningCard::Resolve() {
	if (!owner) return;
	for (const auto& [enemy, damage] : PlanHits()) {
		owner->DealDamage(enemy.get(), damage);
	}
}

void Demo::ChainLightningCard::CollectProjectedSteps(VirtualBattleState& state) {
	for (const auto& [enemy, damage] : PlanHits()) {
		state.SimulateDamage(enemy.get(), damage);
	}
}

// ---- Raging Strike -------------------------------------------------------------------------

std::wstring Demo::RagingStrikeCard::GetDescription() const {
	return Tr(L"Deal ") + std::to_wstring(currentDamage) + Tr(L" damage. Increases this card's damage by 3 when played. Resets to 4 damage on discard.");
}

void Demo::RagingStrikeCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy) return;
	if (owner) owner->DealDamage(enemy.get(), static_cast<float>(currentDamage));
	currentDamage += DAMAGE_GAIN;
}

void Demo::RagingStrikeCard::CollectProjectedSteps(VirtualBattleState& state) {
	CollectHitsOnTargets(state, static_cast<float>(currentDamage), 1);
}

// ---- Overload ------------------------------------------------------------------------------

float Demo::OverloadCard::TotalDamage() const {
	float damage = BASE_DAMAGE;
	if (auto block = GetParentBlock()) {
		for (const auto& weak : block->GetStatementCards()) {
			if (auto card = weak.lock(); card && card->IsPersistent()) damage += DAMAGE_PER_PERSISTENT;
		}
	}
	return damage;
}

void Demo::OverloadCard::Resolve() {
	auto enemy = LiveTarget();
	if (enemy && owner) owner->DealDamage(enemy.get(), TotalDamage());
}

void Demo::OverloadCard::CollectProjectedSteps(VirtualBattleState& state) {
	CollectHitsOnTargets(state, TotalDamage(), 1);
}

// ---- Chain Reaction ------------------------------------------------------------------------

std::shared_ptr<Demo::TargetedCard> Demo::ChainReactionCard::PreviousCard() const {
	auto block = GetParentBlock();
	if (!block) return nullptr;
	const auto& cards = block->GetStatementCards();
	for (size_t i = 1; i < cards.size(); ++i) {
		if (cards[i].lock().get() == this) {
			return std::dynamic_pointer_cast<TargetedCard>(cards[i - 1].lock());
		}
	}
	return nullptr;
}

void Demo::ChainReactionCard::Resolve() {
	const auto attached = GetTargetEnemies();
	if (attached.empty()) return;

	float damage = BASE_DAMAGE;
	IEnemy* target = attached[0].get();

	if (auto previous = PreviousCard()) {
		bool previousKilled = false;
		for (const auto& enemy : previous->GetTargetEnemies()) {
			if (enemy->IsDead()) {
				previousKilled = true;
				break;
			}
		}
		if (previousKilled) {
			damage = REDIRECTED_DAMAGE;
			if (battleScene) {
				float lowestHealth = (std::numeric_limits<float>::max)();
				for (const auto& enemy : battleScene->GetEnemies()) {
					if (!enemy->IsDead() && enemy->GetHealth() < lowestHealth) {
						lowestHealth = enemy->GetHealth();
						target = enemy.get();
					}
				}
			}
		}
	}

	if (owner && !target->IsDead()) owner->DealDamage(target, damage);
}

void Demo::ChainReactionCard::CollectProjectedSteps(VirtualBattleState& state) {
	const auto attached = GetTargetEnemies();
	if (attached.empty()) return;

	float damage = BASE_DAMAGE;
	IEnemy* target = attached[0].get();

	if (auto previous = PreviousCard()) {
		bool previousKilled = false;
		for (const auto& enemy : previous->GetTargetEnemies()) {
			if (state.enemies[enemy.get()].IsDead()) {
				previousKilled = true;
				break;
			}
		}
		if (previousKilled) {
			damage = REDIRECTED_DAMAGE;
			if (IEnemy* lowest = state.GetLowestHPEnemyAlive()) target = lowest;
		}
	}

	state.SimulateDamage(target, damage);
}

// ---- Lethal Harvest, Execute ---------------------------------------------------------------

void Demo::LethalHarvestCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	owner->DealDamage(enemy.get(), 5.f);
	if (enemy->IsDead()) owner->Heal(8.f);
}

void Demo::ExecuteCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	owner->DealDamage(enemy.get(), 15.f);
	if (enemy->IsDead() && battleScene) battleScene->QueueBonusEnergy(1);
}

// ---- Armor Piercer, Cruel Strike, Shield Bash ----------------------------------------------

void Demo::ArmorPiercerCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	float damage = BASE_DAMAGE;
	if (enemy->GetModifierValue(ModifierType::BuffDefense) > 0.f) damage *= 2.f;
	owner->DealDamage(enemy.get(), damage, true);
}

void Demo::ArmorPiercerCard::CollectProjectedSteps(VirtualBattleState& state) {
	float damage = BASE_DAMAGE;
	const auto attached = GetTargetEnemies();
	if (!attached.empty() && state.enemies[attached[0].get()].block > 0.f) damage *= 2.f;
	CollectHitsOnTargets(state, damage, 1);
}

void Demo::CruelStrikeCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	float damage = BASE_DAMAGE;
	if (enemy->HasModifier(ModifierType::Weak)) damage *= 2.f;
	owner->DealDamage(enemy.get(), damage);
}

void Demo::CruelStrikeCard::CollectProjectedSteps(VirtualBattleState& state) {
	float damage = BASE_DAMAGE;
	const auto attached = GetTargetEnemies();
	if (!attached.empty() && state.enemies[attached[0].get()].weak) damage *= 2.f;
	CollectHitsOnTargets(state, damage, 1);
}

void Demo::ShieldBashCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	const float consumedArmor = owner->ConsumeModifier(ModifierType::BuffDefense);
	owner->DealDamage(enemy.get(), consumedArmor * DAMAGE_PER_ARMOR);
}

void Demo::ShieldBashCard::CollectProjectedSteps(VirtualBattleState& state) {
	const float damage = state.player.block * DAMAGE_PER_ARMOR;
	state.player.block = 0.f;
	CollectHitsOnTargets(state, damage, 1);
}

// ---- Terminate -----------------------------------------------------------------------------

void Demo::TerminateCard::Resolve() {
	auto enemy = LiveTarget();
	if (!enemy || !owner) return;
	const float maxHealth = enemy->GetMaxHealth();
	const bool finishable = maxHealth > 0.f && (enemy->GetHealth() / maxHealth) < FINISHER_HEALTH_RATIO;
	owner->DealDamage(enemy.get(), finishable ? FINISHER_DAMAGE : DAMAGE);
}

void Demo::TerminateCard::CollectProjectedSteps(VirtualBattleState& state) {
	const auto attached = GetTargetEnemies();
	if (attached.empty()) return;
	IEnemy* enemy = attached[0].get();
	const float maxHealth = enemy->GetMaxHealth();
	const bool finishable = maxHealth > 0.f && (state.enemies[enemy].health / maxHealth) < FINISHER_HEALTH_RATIO;
	state.SimulateDamage(enemy, finishable ? FINISHER_DAMAGE : DAMAGE);
}
