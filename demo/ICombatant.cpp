#include "pch.h"
#include "ICombatant.h"
#include "DamageTextManager.h"
namespace Demo {
	void Demo::ICombatant::Heal(int value) {
		if (IsDead()) return;

		//heals must be whole
		const float whole = std::round(value);
		const float actualHeal = (std::min)(whole, maxHealth - health);
		health = (std::min)(maxHealth, health + whole);

		if (actualHeal > 0) {
			SpawnHealText(actualHeal);
		}
	}

	void Demo::ICombatant::SpawnHealText(int actualHeal) {
		DamageTextManager::GetInstance()->Spawn(actualHeal, GetWorldX(), GetWorldY() - 40.f, TextType::Heal);
	}

	float ICombatant::CalculateActualDamage(float baseDamage, bool ignoreArmor) {
		// Marked is a flat bonus per hit, so it lands before Vulnerable scales the total and
		// before block eats into it.
		float finalDamage = ScaleIncomingDamage(
			baseDamage,
			GetModifierValue(ModifierType::Marked),
			HasModifier(ModifierType::Vulnerable));

		if (temporaryDefense > 0.f && !ignoreArmor) {
			float blockedDamage = (std::min)(temporaryDefense, finalDamage);
			temporaryDefense -= blockedDamage;
			finalDamage -= blockedDamage;

			float dmgToDeduct = blockedDamage;
			for (auto& mod : modifiers) {
				if (mod.type == ModifierType::BuffDefense && mod.value > 0.f) {
					float deduct = (std::min)(mod.value, dmgToDeduct);
					mod.value -= deduct;
					dmgToDeduct -= deduct;
					if (dmgToDeduct <= 0.f) break;
				}
			}
		}

		return finalDamage;
	}

	float ICombatant::CalculateOutgoingDamage(float baseDamage) const {
		return ScaleOutgoingDamage(
			baseDamage,
			GetModifierValue(ModifierType::BuffDamage),
			HasModifier(ModifierType::Weak));
	}

	void ICombatant::AddModifier(ModifierType type, int duration, float value, bool isBuff, int delayTurns) {
		if (!isBuff && TryBlockWithImmunity()) return;
		for (auto& mod : modifiers) {
			if (mod.type == type) {
				mod.duration += duration;
				mod.value = (std::max)(mod.value, value);
				if (type == ModifierType::BuffDefense) {
					temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
				}
				return;
			}
		}
		modifiers.push_back({ type, duration, value, isBuff, delayTurns });
		if (type == ModifierType::BuffDefense) {
			temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
		}
	}

	void ICombatant::AddStackingModifier(ModifierType type, int duration, float value, bool isBuff, int delayTurns) {
		if (!isBuff && TryBlockWithImmunity()) return;
		for (auto& mod : modifiers) {
			if (mod.type == type) {
				mod.value += value;
				mod.duration = (std::max)(mod.duration, duration);
				if (type == ModifierType::BuffDefense) {
					temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
				}
				return;
			}
		}
		modifiers.push_back({ type, duration, value, isBuff, delayTurns });
		if (type == ModifierType::BuffDefense) {
			temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
		}
	}

	bool ICombatant::HasModifier(ModifierType type) const {
		for (const auto& mod : modifiers) {
			if (mod.type == type && mod.duration > 0) return true;
		}
		return false;
	}

	float ICombatant::GetModifierValue(ModifierType type) const {
		float val = 0.f;
		for (const auto& mod : modifiers) {
			if (mod.type == type) {
				val += mod.value;
			}
		}
		return val;
	}

	int ICombatant::GetModifierDuration(ModifierType type) const {
		int turns = 0;
		for (const auto& mod : modifiers) {
			if (mod.type == type && mod.duration > 0) turns += mod.duration;
		}
		return turns;
	}

	float ICombatant::RemoveBlock(float amount) {
		const float removed = (std::min)(amount, temporaryDefense);
		if (removed <= 0.f) return 0.f;

		temporaryDefense -= removed;
		// Same bookkeeping as a hit absorbed by block: the BuffDefense modifier shrinks with it.
		float toDeduct = removed;
		for (auto& mod : modifiers) {
			if (mod.type == ModifierType::BuffDefense && mod.value > 0.f) {
				const float deduct = (std::min)(mod.value, toDeduct);
				mod.value -= deduct;
				toDeduct -= deduct;
				if (toDeduct <= 0.f) break;
			}
		}
		return removed;
	}

	bool ICombatant::TryBlockWithImmunity() {
		for (auto& mod : modifiers) {
			if (mod.type == ModifierType::Immunity && mod.duration > 0) {
				mod.value -= 1.0f;
				if (mod.value <= 0.f) {
					mod.duration = 0; 
				}
				return true;
			}
		}
		return false;
	}

	bool ICombatant::TakeIndirectDamage(float damage, DamageType type) {
		health -= std::round(damage);
		if (health < 0) health = 0;
		return IsDead();
	}

	void ICombatant::TriggerEffects(TickPhase phase) {
		for (auto& it : modifiers) {
			if (it.duration > 0) {
				if (it.type == ModifierType::Poison && phase == TickPhase::EndOfTurn) {
					if (TryBlockWithImmunity()) continue;
					const float poisonDamage = (it.value > 0.f) ? it.value : static_cast<float>(it.duration);

					float tempDef = temporaryDefense;
					temporaryDefense = 0.f;

					int vulnDuration = 0;
					for (auto& m : modifiers) {
						if (m.type == ModifierType::Vulnerable) {
							vulnDuration = m.duration;
							m.duration = 0;
						}
					}

					this->TakeIndirectDamage(poisonDamage, DamageType::Poison);

					temporaryDefense = tempDef;
					for (auto& m : modifiers) {
						if (m.type == ModifierType::Vulnerable && vulnDuration > 0) {
							m.duration = vulnDuration;
						}
					}
				}
				// Burn and Regen need none of the dance above: TakeIndirectDamage already
				// bypasses block and modifiers, and Heal is unaffected by both.
				else if (it.type == ModifierType::Burn && phase == TickPhase::EndOfTurn) {
					if (TryBlockWithImmunity()) continue;
					this->TakeIndirectDamage(it.value, DamageType::Burn);
				}
				else if (it.type == ModifierType::Regen && phase == TickPhase::EndOfTurn) {
					this->Heal(it.value);
				}
			}
		}
	}

	void ICombatant::TickDurations(TickPhase phase) {
		if (phase != TickPhase::EndOfRound) return;

		for (auto it = modifiers.begin(); it != modifiers.end(); ) {
			// Grace turns are spent instead of duration, so a buff applied this round keeps every
			// turn it advertised - it is already in effect, it just does not age yet.
			if (it->delayTurns > 0) {
				it->delayTurns--;
				++it;
				continue;
			}

			it->duration--;

			if (it->duration <= 0 || (it->type == ModifierType::BuffDefense && it->value <= 0.f)) {
				it = modifiers.erase(it);
			}
			else {
				++it;
			}
		}
		temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
	}

	float ICombatant::ConsumeModifier(ModifierType type) {
		float totalVal = 0.f;
		for (auto it = modifiers.begin(); it != modifiers.end(); ) {
			if (it->type == type) {
				totalVal += it->value;
				it = modifiers.erase(it);
			}
			else {
				++it;
			}
		}
		if (type == ModifierType::BuffDefense) {
			temporaryDefense = (std::max)(0.f, GetModifierValue(ModifierType::BuffDefense));
		}
		return totalVal;
	}

	float ICombatant::ConsumeAllArmor() {
		return ConsumeModifier(ModifierType::BuffDefense);
	}

	void ICombatant::ClearBuffs() {
		modifiers.erase(
			std::remove_if(modifiers.begin(), modifiers.end(),
				[](const CombatModifier& mod) { return mod.isBuff; }),
			modifiers.end());
	}

	void Demo::ICombatant::ClearDebuffs() {
		modifiers.erase(std::remove_if(modifiers.begin(), modifiers.end(),
			[](const CombatModifier& mod) {
				return !mod.isBuff;
			}),
			modifiers.end());
	}

	float ICombatant::ScaleOutgoingDamage(float baseDamage, float buffDamage, bool weak) {
		float damage = baseDamage + buffDamage;
		if (weak && damage > 0.f) {
			// Rounded per hit to match how each card resolves on its own. A hit that had
			// damage never drops to 0.
			damage = (std::max)(1.f, std::round(damage * 0.75f));
		}
		return damage;
	}

	float ICombatant::ScaleIncomingDamage(float damage, float marked, bool vulnerable) {
		// Marked is a flat bonus per hit and lands before Vulnerable scales the total.
		float scaled = damage + marked;
		if (vulnerable) {
			scaled *= 1.5f;
		}
		return scaled;
	}
}