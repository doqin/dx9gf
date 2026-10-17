#pragma once
#include "CardEffect.h"
#include "CardFrame.h"
#include "GameItems.h"
#include <optional>
#include <string>
#include <vector>

namespace Demo {
	// The static definition of a card: what it costs, how it looks and what it does. A concrete
	// card passes one of these to its base class and only adds code for logic that effects cannot
	// express.
	struct CardSpec {
		size_t cost = 0;
		CardTemplate face = CardTemplate::Red;
		// English text; translated with Tr() when shown.
		std::wstring description;
		// Persistent cards stay in their block and re-execute every turn until the program clears.
		// Others fire once per placement and return to the discard pile.
		bool persistent = true;
		// Cap on executions per battle. Empty means unlimited.
		std::optional<int> maxUses;
		std::vector<CardEffect> effects;
		// Execution steps the effects are applied over, one application per step.
		int repeat = 1;
		// Status effects shown in the tooltip for cards whose logic is not written as effects.
		std::vector<AppliedStatusEffect> extraStatusEffects;
	};
}
