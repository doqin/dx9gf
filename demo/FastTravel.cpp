#include "pch.h"
#include "FastTravel.h"
#include "PlayerGlobalData.h"
#include "SpamEnemy.h"
#include "CupidEnemy.h"
#include "TrojanEnemy.h"
#include "KeyeproEnemy.h"

namespace Demo::FastTravel {
	const std::vector<Destination>& Destinations()
	{
		// One row per map. The start point and music match that scene's InitCore and the portals into it.
		static const std::vector<Destination> destinations = {
			{ "TutorialWorldScene", L"Cloud Canopy", 248.f,        184.f,        "bgm_tutorial",  0.5f, typeid(SpamEnemy) },
			{ "SecretPuzzleScene",  L"The Root",     -84.f * 16.f, -39.f * 16.f, "bgm_secret",    0.3f, typeid(CupidEnemy) },
			{ "ThreadAlleyScene",   L"Thread Alley", -544.5f,      128.5f,       "bgm_arcade",    0.2f, typeid(TrojanEnemy) },
			{ "BossWorldScene",     L"The Overflow", 360.f,        190.f,        "bgm_bossworld", 0.3f, typeid(KeyeproEnemy) },
		};
		return destinations;
	}

	const Destination* UnlockByBoss(std::type_index bossType)
	{
		auto data = PlayerGlobalData::GetInstance();
		for (const auto& dest : Destinations()) {
			if (dest.boss != bossType || data->IsFastTravelUnlocked(dest.sceneId)) continue;
			data->UnlockFastTravel(dest.sceneId);
			return &dest;
		}
		return nullptr;
	}
}