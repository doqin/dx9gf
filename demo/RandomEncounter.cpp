#include "pch.h"
#include "RandomEncounter.h"
#include <random>
#include <chrono>
#include "CustomBattleScene.h"
#include "Game.h"

std::function<void(const DX9GF::Map::ObjectArea&)> Demo::GetRandomEncounterFunc(
	DX9GF::IGame* game,
	std::shared_ptr<Player> player,
	std::vector<std::pair<std::string, int>> possibleEnemies
)
{
	auto lastEncounterTime = std::chrono::steady_clock::now();
	const int COOLDOWN_SECONDS = 3;

	return [game, player, possibleEnemies, lastEncounterTime, COOLDOWN_SECONDS](const DX9GF::Map::ObjectArea& area) mutable {
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - lastEncounterTime).count() < COOLDOWN_SECONDS) {
			return;
		}

		if (player->IsWalking()) {
			const int CHANCE = 3; // in percentage
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dis(1, 10000);

			if (dis(gen) <= 10000 / 100 * CHANCE) {
				lastEncounterTime = now;
				std::map<std::string, int> possibleEnemiesMap;
                for (const auto& [name, chance] : possibleEnemies) {
                    possibleEnemiesMap[name] = chance;
                }                
                auto demoGame = dynamic_cast<Demo::Game*>(game);
				auto app = DX9GF::Application::GetInstance();
                auto battleScene = new CustomBattleScene(demoGame, player, app->GetScreenWidth(), app->GetScreenHeight(), possibleEnemiesMap);
                
                auto sceMan = game->GetSceneManager();
                sceMan->PushScene(battleScene);
                sceMan->GoToNext();
            }
        }
    };
}
