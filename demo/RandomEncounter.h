#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"
#include <functional>

namespace Demo {
	std::function<void(const DX9GF::Map::ObjectArea&)> GetRandomEncounterFunc(
		DX9GF::IGame* game, 
		std::shared_ptr<Player>, 
		std::vector<std::pair<std::string, int>> possibleEnemies
	);
}