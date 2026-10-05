#include "pch.h"
#include "EnemyFactory.h"
#include "EncounterGenerator.h"
#include "TestEnemy.h"
#include "DemonEyeEnemy.h"
#include "VampireBatEnemy.h"
#include "MimicEnemy.h"
#include "WarlockEnemy.h"
#include "CupidEnemy.h"
#include "KeyeEnemy.h"
#include "KeyeproEnemy.h"
#include "KernelEnemy.h"
#include "TrojanEnemy.h"
#include "TuitionFeeEnemy.h"
#include "HomeworkEnemy.h"
#include "DeadlineEnemy.h"
#include "RNG.h"

namespace {
	template <typename T>
	std::shared_ptr<Demo::IEnemy> MakeEnemy(std::weak_ptr<DX9GF::TransformManager> tm, int hp,
		DX9GF::GraphicsDevice* gd, DX9GF::Camera* cam) {
		auto enemy = std::make_shared<T>(tm, hp);
		enemy->Init(gd, cam);
		return enemy;
	}
}

namespace Demo {

	OverworldSpriteData EnemyFactory::GetOverworldSprite(const std::string& enemyType) {
		if (enemyType == "KeyeEnemy")       return { L"assets/minion-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "DemonEyeEnemy")   return { L"assets/computerbug-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "MimicEnemy")      return { L"assets/notresponding-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "CupidEnemy")      return { L"assets/bubble-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "VampireBatEnemy") return { L"assets/shrimp-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "WarlockEnemy")    return { L"assets/crawler-Sheet.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "KernelEnemy")      return { L"assets/kernel_2.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "TrojanEnemy")     return { L"assets/Trojan_outside.png", 64.f, 64.f, 12, 38.f, 45.f };
		if (enemyType == "TuitionFeeEnemy") return { L"assets/placeholder.png", 64.f, 64.f, 8, 38.f, 45.f }; // TODO: change to the real asset path
		if (enemyType == "DeadlineEnemy") return { L"assets/placeholder.png", 64.f, 64.f, 8, 38.f, 45.f }; // TODO: change to the real asset path
		if (enemyType == "HomeworkEnemy") return { L"assets/placeholder.png", 64.f, 64.f, 8, 38.f, 45.f }; // TODO: change to the real asset path
		return { L"assets/random-Sheet.png", 64.f, 64.f,8, 38.f, 45.f };
	}

	bool EnemyFactory::TryGetHpRange(const std::string& type, int& lo, int& hi) {
		if (type == "TestEnemy") { lo = 40;  hi = 60;  return true; }
		if (type == "DemonEyeEnemy") { lo = 25;  hi = 45;  return true; }
		if (type == "VampireBatEnemy") { lo = 60;  hi = 80;  return true; }
		if (type == "MimicEnemy") { lo = 60;  hi = 80;  return true; }
		if (type == "WarlockEnemy") { lo = 70;  hi = 90;  return true; }
		if (type == "KeyeEnemy") { lo = 20;  hi = 40;  return true; }
		if (type == "KernelEnemy") { lo = 40;  hi = 95;  return true; }
		if (type == "CupidEnemy") { lo = 200; hi = 200; return true; }
		if (type == "KeyeproEnemy") { lo = 500; hi = 500; return true; }
		if (type == "TrojanEnemy") { lo = 40;  hi = 60;  return true; }
		if (type == "TuitionFeeEnemy") { lo = 140; hi = 140; return true; }
		if (type == "DeadlineEnemy") { lo = 25;  hi = 32;  return true; }
		if (type == "HomeworkEnemy") { lo = 100; hi = 105; return true; }
		return false;
	}

	int EnemyFactory::RollMaxHp(const std::string& type) {
		int lo = 0, hi = 0;
		if (!TryGetHpRange(type, lo, hi)) return 0;
		const int rolled = (lo == hi) ? lo : RNG::Range(lo, hi);
		// Health is displayed and damaged in whole numbers, so it is rolled as one.
		return (std::max)(1, static_cast<int>(std::round(rolled)));
	}

	std::vector<int> EnemyFactory::RollEncounterHps(const std::vector<std::string>& types) {
		std::vector<int> hps;
		hps.reserve(types.size());
		for (const auto& type : types) {
			hps.push_back(RollMaxHp(type));
		}
		return hps;
	}

	std::shared_ptr<IEnemy> EnemyFactory::Create(const std::string& type, std::weak_ptr<DX9GF::TransformManager> tm,
		DX9GF::GraphicsDevice* gd, DX9GF::Camera* cam, int maxHp) {
		int lo = 0, hi = 0;
		if (!TryGetHpRange(type, lo, hi)) return nullptr;

		const int hp = static_cast<int>(maxHp > 0 ? maxHp : RollMaxHp(type));

		if (type == "TestEnemy")       return MakeEnemy<TestEnemy>(tm, hp, gd, cam);
		if (type == "DemonEyeEnemy")   return MakeEnemy<DemonEyeEnemy>(tm, hp, gd, cam);
		if (type == "VampireBatEnemy") return MakeEnemy<VampireBatEnemy>(tm, hp, gd, cam);
		if (type == "MimicEnemy")      return MakeEnemy<MimicEnemy>(tm, hp, gd, cam);
		if (type == "WarlockEnemy")    return MakeEnemy<WarlockEnemy>(tm, hp, gd, cam);
		if (type == "KeyeEnemy")       return MakeEnemy<KeyeEnemy>(tm, hp, gd, cam);
		if (type == "KernelEnemy")     return MakeEnemy<KernelEnemy>(tm, hp, gd, cam);
		if (type == "CupidEnemy")      return MakeEnemy<CupidEnemy>(tm, hp, gd, cam);
		if (type == "KeyeproEnemy")    return MakeEnemy<KeyeproEnemy>(tm, hp, gd, cam);
		if (type == "TrojanEnemy")     return MakeEnemy<TrojanEnemy>(tm, hp, gd, cam);
		if (type == "TuitionFeeEnemy") return MakeEnemy<TuitionFeeEnemy>(tm, hp, gd, cam);
		if (type == "DeadlineEnemy")   return MakeEnemy<DeadlineEnemy>(tm, hp, gd, cam);
		if (type == "HomeworkEnemy")   return MakeEnemy<HomeworkEnemy>(tm, hp, gd, cam);
		return nullptr;
	}

	std::shared_ptr<MapEnemy> EnemyFactory::CreateMapEnemy(
		float x, float y, const std::string& id,
		const std::vector<std::string>& types, bool isRandomPool, bool useGlobalPool,
		const std::string& bgmName,
		std::function<void(DX9GF::GraphicsDevice*, unsigned long long)> bgDrawFunc,
		std::shared_ptr<DX9GF::TransformManager> tm, Game* game, DX9GF::ColliderManager* colMan, std::shared_ptr<Player> player)
	{
		BattleEncounter enc;
		enc.mapEnemyID = id;
		enc.useGlobalPool = useGlobalPool;

		if (useGlobalPool) enc.enemyTypes = EncounterGenerator::GenerateNormalEncounter();
		else if (isRandomPool) {
			enc.randomPool = types;
			enc.enemyTypes = EncounterGenerator::GenerateFromTypes(enc.randomPool);
		}
		else enc.enemyTypes = types;

		enc.enemyHps = RollEncounterHps(enc.enemyTypes);

		enc.bgmName = bgmName;
		enc.bgDrawFunc = bgDrawFunc;

		std::string spriteType = (isRandomPool || useGlobalPool) ? "Random" : enc.enemyTypes[0];
		auto sprite = GetOverworldSprite(spriteType);
		enc.mapTexturePath = sprite.texturePath;
		enc.spriteWidth = static_cast<int>(sprite.width);
		enc.spriteHeight = static_cast<int>(sprite.height);
		enc.frameCount = sprite.frameCount;
		enc.hitBoxWidth = sprite.hitBoxWidth;
		enc.hitBoxHeight = sprite.hitBoxHeight;

		auto enemy = std::make_shared<MapEnemy>(tm, x, y, enc);
		enemy->Init(game, game->GetGraphicsDevice(), colMan, player);
		return enemy;
	}
}