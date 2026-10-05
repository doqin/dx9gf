#include "pch.h"
#include "MapBattleScene.h"
#include "EnemyFactory.h"
#include "RNG.h"

namespace Demo {

    MapBattleScene::MapBattleScene(Game* game, std::shared_ptr<Player> player, int screenWidth, int screenHeight, const BattleEncounter& enc)
        : IBattleScene(game, player, screenWidth, screenHeight), encounter(enc) {
    }

    void MapBattleScene::Init() {
        this->battleEventType = encounter.eventType;
        // Roaming-enemy battles are where the player first meets the card system; IBattleScene
        // shows the walkthrough here only if it hasn't been seen yet.
        this->tutorialEnabled = true;
        IBattleScene::Init();
        std::string finalBGM = encounter.bgmName;

        // regular (non-boss) encounters all share the intro + loop battle track
        if (finalBGM.rfind("battle_loop", 0) == 0) {
            finalBGM = "battle_bgm";
        }

        this->SetCustomBGM(finalBGM);
        if (encounter.bgDrawFunc) {
            this->SetCustomBackgroundDraw(encounter.bgDrawFunc);
        }

        const float spacingY = 140.f;
        const size_t enemyCount = encounter.enemyTypes.size();

        for (size_t i = 0; i < enemyCount; ++i) {
            const int lockedHp = (i < encounter.enemyHps.size()) ? encounter.enemyHps[i] : -1;
            auto enemy = EnemyFactory::Create(encounter.enemyTypes[i], transformManager, game->GetGraphicsDevice(), &camera, lockedHp);
            if (enemy) {
                enemy->SetOnRequestEnemyCard([this](std::shared_ptr<IEnemy> e) {
                    this->CreateEnemyCard(e);
                    });
                // onRequestLockCard is wired centrally in IBattleScene::StartBattle()
                this->enemies.push_back(enemy);
            }
        }

        transformManager->RebuildHierarchy();
        this->StartBattle();
    }
}