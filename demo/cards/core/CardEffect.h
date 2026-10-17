#pragma once
#include "GameItems.h"

namespace Demo {
	enum class EffectScope {
		Targets,    // the enemy cards attached to the statement card
		AllEnemies, // every living enemy, no enemy card needed
		Self        // the card's owner
	};

	// One thing a card does when it executes. The same description drives the real execution and
	// the damage preview drawn on the enemies, so the two cannot drift apart.
	struct CardEffect {
		enum class Kind { Damage, Modifier };

		Kind kind = Kind::Damage;
		EffectScope scope = EffectScope::Targets;
		float damage = 0.f;
		ModifierType modifier{};
		int turns = 0;
		float value = 0.f;
		// Mirrors ICombatant::AddStackingModifier (value adds, duration takes the longer) versus
		// AddModifier (duration adds, value takes the larger).
		bool stacking = false;
		// Targets scope only: how many attached targets are affected, 0 for all of them.
		size_t maxTargets = 1;
	};

	namespace Effect {
		inline CardEffect Hit(float damage, size_t maxTargets = 1) {
			return { .kind = CardEffect::Kind::Damage, .scope = EffectScope::Targets, .damage = damage, .maxTargets = maxTargets };
		}
		inline CardEffect HitAll(float damage) {
			return { .kind = CardEffect::Kind::Damage, .scope = EffectScope::AllEnemies, .damage = damage };
		}
		inline CardEffect Apply(ModifierType type, int turns, float value = 0.f, size_t maxTargets = 1) {
			return { .kind = CardEffect::Kind::Modifier, .scope = EffectScope::Targets, .modifier = type,
				.turns = turns, .value = value, .maxTargets = maxTargets };
		}
		inline CardEffect ApplyStacking(ModifierType type, int turns, float value = 0.f, size_t maxTargets = 1) {
			return { .kind = CardEffect::Kind::Modifier, .scope = EffectScope::Targets, .modifier = type,
				.turns = turns, .value = value, .stacking = true, .maxTargets = maxTargets };
		}
		inline CardEffect ApplyAll(ModifierType type, int turns, float value = 0.f) {
			return { .kind = CardEffect::Kind::Modifier, .scope = EffectScope::AllEnemies, .modifier = type,
				.turns = turns, .value = value };
		}
		inline CardEffect ApplyAllStacking(ModifierType type, int turns, float value = 0.f) {
			return { .kind = CardEffect::Kind::Modifier, .scope = EffectScope::AllEnemies, .modifier = type,
				.turns = turns, .value = value, .stacking = true };
		}
		// A buff the card's owner gains. Always stacks, so two copies in one program add up.
		inline CardEffect Gain(ModifierType type, int turns, float value = 0.f) {
			return { .kind = CardEffect::Kind::Modifier, .scope = EffectScope::Self, .modifier = type,
				.turns = turns, .value = value, .stacking = true };
		}
	}
}
