#include "pch.h"
#include "ICard.h"
#include <algorithm>

std::vector<Demo::AppliedStatusEffect> Demo::ICard::GetAppliedStatusEffects() const {
	std::vector<AppliedStatusEffect> result;
	for (const auto& effect : spec.effects) {
		if (effect.kind != CardEffect::Kind::Modifier) continue;
		const bool listed = std::any_of(result.begin(), result.end(),
			[&](const AppliedStatusEffect& e) { return e.type == effect.modifier; });
		if (!listed) result.push_back({ effect.modifier, effect.stacking });
	}
	result.insert(result.end(), spec.extraStatusEffects.begin(), spec.extraStatusEffects.end());
	return result;
}
