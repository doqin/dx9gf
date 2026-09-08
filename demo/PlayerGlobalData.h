#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "GameItems.h"
#include <string>
#include <vector>
#include <algorithm>

namespace Demo {
	// Single source of truth for save-scoped player state (health, gold, deck, inventory).
	// Every scene constructs its own Player instance, and battles construct a further transient
	// battlePlayer on top of that - routing this state through one global instance means all of
	// them automatically share it, instead of needing to be manually copied around.
	class PlayerGlobalData : public DX9GF::ISaveable {
	private:
		float baseMaxHealth = 50.f;
		float health = 50.f;
		int gold = 100;
		std::vector<std::string> deck;
		std::vector<std::string> inventoryCards;
		ItemInventory inventoryItems;
		std::vector<int> inventoryGears;
		int equippedActiveGearID = -1;
		int equippedPassiveGearID = -1;
		bool showGearOnMap = true;
		// Set once the player has been walked through the first battle, so the walkthrough
		// never replays. Persisted with the save.
		bool seenBattleTutorial = false;
		PlayerGlobalData() { Reset(); }
	public:
		static PlayerGlobalData* GetInstance() {
			static PlayerGlobalData instance;
			return &instance;
		}

		void Reset();

		float GetHealth() const { return health; }
		float GetMaxHealth() const;
		void SetHealth(float hp) { health = (std::min)(GetMaxHealth(), (std::max)(0.f, hp)); }
		bool IsDead() const { return health <= 0.f; }
		float Heal(float value);

		const std::vector<int>& GetInventoryGears() const { return inventoryGears; }
		int GetEquippedActiveGearID() const { return equippedActiveGearID; }
		int GetEquippedPassiveGearID() const { return equippedPassiveGearID; }
		bool GetShowGearOnMap() const { return showGearOnMap; }
		void ToggleShowGearOnMap() { showGearOnMap = !showGearOnMap; }

		bool HasSeenBattleTutorial() const { return seenBattleTutorial; }
		void SetSeenBattleTutorial(bool seen) { seenBattleTutorial = seen; }
		bool HasGearEquipped(int gearID) const { return equippedActiveGearID == gearID || equippedPassiveGearID == gearID; }

		void EquipGear(int gearID);
		void UnequipGear(int gearID);
		void AddGear(int gearID) { inventoryGears.push_back(gearID); }

		int GetGold() const { return gold; }
		void SetGold(int amount) { gold = amount; }
		void AddGold(int amount) { gold += amount; }

		const std::vector<std::string>& GetDeck() const { return deck; }
		void AddCardToDeck(const std::string& card) { deck.push_back(card); }
		void ClearDeck() { deck.clear(); }

		const std::vector<std::string>& GetInventoryCards() const { return inventoryCards; }
		void AddCardToInventory(const std::string& card) { inventoryCards.push_back(card); }
		// Removes a single copy of the named card from the inventory (not the deck). Returns
		// true if one was found and erased. Used by the card shop's sell mode.
		bool RemoveCardFromInventory(const std::string& card) {
			auto it = std::find(inventoryCards.begin(), inventoryCards.end(), card);
			if (it == inventoryCards.end()) return false;
			inventoryCards.erase(it);
			return true;
		}
		void ClearInventory() { inventoryCards.clear(); }

		ItemInventory& GetInventoryItems() { return inventoryItems; }

		std::string GetSaveID() const override;
		void GenerateSaveData(nlohmann::json& outData) override;
		void RestoreSaveData(const nlohmann::json& inData) override;
	};
}
