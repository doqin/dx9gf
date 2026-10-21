#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "LocalizationManager.h"
#include <algorithm>
#include <map>
#include <string>

namespace Demo {
	// Defeats from which the boss's hint shows up on the DEFEATED popup.
	inline int BossHintDeaths(const std::string& bossId) {
		if (bossId == "SpamEnemy")    return 1;
		if (bossId == "TrojanEnemy")  return 2;
		if (bossId == "CupidEnemy")   return 2;
		if (bossId == "KeyeproEnemy") return 3;
		return 2;
	}

	// How many times the player has lost to each boss, keyed by the boss enemy's id. A defeat
	// usually ends in "load your last savepoint", which would roll a normally saved counter back to
	// the save's value - so on load the higher of the two counts is kept, and the count only resets
	// when a new game starts.
	class BossDeathGlobalData : public DX9GF::ISaveable {
	private:
		std::map<std::string, int> deaths; // boss enemy id -> defeats

		BossDeathGlobalData() = default;
	public:
		static BossDeathGlobalData* GetInstance() {
			static BossDeathGlobalData instance;
			return &instance;
		}

		void Reset() { deaths.clear(); }

		int GetDeaths(const std::string& bossId) const {
			auto it = deaths.find(bossId);
			return it != deaths.end() ? it->second : 0;
		}
		void AddDeath(const std::string& bossId) { deaths[bossId]++; }

		std::string GetSaveID() const override { return "BossDeathGlobalData"; }
		void GenerateSaveData(nlohmann::json& outData) override {
			outData["deaths"] = deaths;
		}
		void RestoreSaveData(const nlohmann::json& inData) override {
			if (!inData.contains("deaths")) return;
			const auto saved = inData["deaths"].get<std::map<std::string, int>>();
			for (const auto& [bossId, count] : saved) {
				deaths[bossId] = (std::max)(deaths[bossId], count);
			}
		}
	};

	// How to beat each boss, shown once the player keeps losing to it. Empty for anything else.
	inline std::wstring GetBossHint(const std::string& bossId) {
		if (bossId == "SpamEnemy")
			return Tr(L"Spam heals or casts Weak every 3 turns, so finish it with your strongest cards. Follow the gap in the falling mail, keep circling during the spiral, and step out of pop-up clusters before they close in.");
		if (bossId == "TrojanEnemy")
			return Tr(L"Trojan's shots inflict Burn and Freeze, so keep moving. When it raises its 60 Defense shield, use Armor Piercer or Vulnerable instead of plain attacks.");
		if (bossId == "CupidEnemy")
			return Tr(L"Pacman heals or breaks your guard every 3 turns. Its homing shots inflict Poison: sidestep late so they overshoot, and bring healing potions.");
		if (bossId == "KeyeproEnemy")
			return Tr(L"Keyepro summons Keye minions and locks your cards. Clear the minions early, keep spare cards for locked turns, and save burst damage for when it is low - that is when it heals and fortifies.");
		return L"";
	}


	// The boss's own idle animation, shown on the hint popup. Same sheets and frame layout the
	// boss and its overworld NPC already use.
	struct BossPortrait {
		const wchar_t* sheet = nullptr;
		long frameSize = 0;
		long frameCount = 0;
	};

	inline BossPortrait GetBossPortrait(const std::string& bossId) {
		if (bossId == "SpamEnemy")    return { L"assets/spam-Sheet.png", 64, 12 };
		if (bossId == "TrojanEnemy")  return { L"assets/trojan.png", 96, 12 };
		if (bossId == "CupidEnemy")   return { L"assets/bubble-Sheet.png", 64, 11 };
		if (bossId == "KeyeproEnemy") return { L"assets/boss-Sheet.png", 128, 6 };
		return {};
	}
}