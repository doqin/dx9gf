#pragma once
#include <string>
#include <utility>
#include <vector>

namespace Demo::EnemyDialogue {
	// Everything one enemy type says in battle (#348). Text is the English source; it is passed
	// through Tr() when shown, like every other string in the game.
	struct Lines {
		std::vector<std::wstring> intro; // battle start, one picked at random
		std::wstring hurt;               // the first time it drops to half health
		std::wstring taunt;              // the turn after the player drops to half health
		// Ability popup text, exactly as the enemy passes it to Tr() in CastAbility -> what the enemy
		// says instead. A matching ability shows this line in the speech bubble instead of the popup.
		std::vector<std::pair<std::wstring, std::wstring>> abilities;
	};

	// Lines for an enemy type, named like EnemyFactory ("SpamEnemy", "KeyeEnemy", ...).
	// Null for a type with no dialogue.
	const Lines* Get(const std::string& enemyType);
}