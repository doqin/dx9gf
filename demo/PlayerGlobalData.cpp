#include "pch.h"
#include "PlayerGlobalData.h"

float Demo::PlayerGlobalData::GetMaxHealth() const {
	float total = baseMaxHealth;
	auto checkGear = [&](int id) {
		if (id != -1) {
			auto* gear = ItemData::GetInstance()->GetGearBlueprint(id);
			if (gear && gear->effect == GearEffect::AddMaxHP) total += gear->effectValue;
		}
		};
	checkGear(equippedActiveGearID);
	checkGear(equippedPassiveGearID);
	return total;
}

void Demo::PlayerGlobalData::EquipGear(int gearID) {
	auto it = std::find(inventoryGears.begin(), inventoryGears.end(), gearID);
	if (it == inventoryGears.end()) return;

	auto bp = ItemData::GetInstance()->GetGearBlueprint(gearID);
	if (!bp) return;

	inventoryGears.erase(it);

	//automatically sort gear's type
	if (bp->type == GearType::Active) {
		if (equippedActiveGearID != -1) inventoryGears.push_back(equippedActiveGearID);
		equippedActiveGearID = gearID;
	}
	else {
		if (equippedPassiveGearID != -1) inventoryGears.push_back(equippedPassiveGearID);
		equippedPassiveGearID = gearID;
	}
	SetHealth(health);
}

void Demo::PlayerGlobalData::UnequipGear(int gearID) {
	if (gearID == -1) return;
	if (equippedActiveGearID == gearID) {
		inventoryGears.push_back(equippedActiveGearID);
		equippedActiveGearID = -1;
	}
	else if (equippedPassiveGearID == gearID) {
		inventoryGears.push_back(equippedPassiveGearID);
		equippedPassiveGearID = -1;
	}
	SetHealth(health);
}

void Demo::PlayerGlobalData::Reset() {
	baseMaxHealth = 50.f;
	health = baseMaxHealth;
	gold = 100;
	deck = { "StrikeCard", "StrikeCard", "StrikeCard", "TwinStrikeCard", "TwinStrikeCard" };
	inventoryCards.clear();
	inventoryItems = ItemInventory();
	inventoryItems.InitFixedInventory(13);
	inventoryGears.clear();
	equippedActiveGearID = -1;
	equippedPassiveGearID = -1;
	showGearOnMap = true;
	seenBattleTutorial = false;
}

float Demo::PlayerGlobalData::Heal(float value) {
	if (IsDead()) return 0.f;

	float currentMaxHealth = GetMaxHealth();
	float actualHeal = value;
	if (health + value > currentMaxHealth) {
		actualHeal = currentMaxHealth - health;
	}

	SetHealth(health + value);
	return actualHeal;
}

std::string Demo::PlayerGlobalData::GetSaveID() const {
	return "PlayerGlobalData";
}

void Demo::PlayerGlobalData::GenerateSaveData(nlohmann::json& outData) {
	outData["gold"] = gold;
	outData["health"] = health;
	outData["equippedActiveGearID"] = equippedActiveGearID;
	outData["equippedPassiveGearID"] = equippedPassiveGearID;
	outData["showGearOnMap"] = showGearOnMap;
	outData["seenBattleTutorial"] = seenBattleTutorial;

	outData["inventoryGears"] = nlohmann::json::array();
	for (int gear : inventoryGears) {
		outData["inventoryGears"].push_back(gear);
	}

	outData["deck"] = nlohmann::json::array();
	for (auto& card : deck) {
		outData["deck"].push_back(card);
	}
	outData["inventoryCards"] = nlohmann::json::array();
	for (auto& card : inventoryCards) {
		outData["inventoryCards"].push_back(card);
	}
	auto inventorySlots = inventoryItems.GetSlots();
	for (size_t i = 0; i < inventorySlots.size(); i++) {
		outData["inventoryItems"][i]["id"] = inventorySlots[i].itemID;
		outData["inventoryItems"][i]["quantity"] = inventorySlots[i].quantity;
	}
}

void Demo::PlayerGlobalData::RestoreSaveData(const nlohmann::json& inData) {
	if (inData.contains("gold")) gold = inData["gold"];
	if (inData.contains("showGearOnMap")) showGearOnMap = inData["showGearOnMap"];
	if (inData.contains("seenBattleTutorial")) seenBattleTutorial = inData["seenBattleTutorial"];

	if (inData.contains("equippedGearID")) {
		int oldGearID = inData["equippedGearID"];
		if (oldGearID != -1) {
			auto bp = ItemData::GetInstance()->GetGearBlueprint(oldGearID);
			if (bp && bp->type == GearType::Active) equippedActiveGearID = oldGearID;
			else equippedPassiveGearID = oldGearID;
		}
	}
	else {
		if (inData.contains("equippedActiveGearID")) equippedActiveGearID = inData["equippedActiveGearID"];
		if (inData.contains("equippedPassiveGearID")) equippedPassiveGearID = inData["equippedPassiveGearID"];
	}
	if (inData.contains("inventoryGears")) {
		inventoryGears.clear();
		for (auto& item : inData["inventoryGears"]) {
			inventoryGears.push_back(item.get<int>());
		}
	}
	if (inData.contains("health")) health = inData["health"];
	if (inData.contains("deck")) {
		deck.clear();
		for (auto& item : inData["deck"]) {
			deck.push_back(item.get<std::string>());
		}
	}
	if (inData.contains("inventoryCards")) {
		inventoryCards.clear();
		for (auto& item : inData["inventoryCards"]) {
			inventoryCards.push_back(item.get<std::string>());
		}
	}
	if (inData.contains("inventoryItems")) {
		inventoryItems.Clear();
		for (auto& item : inData["inventoryItems"]) {
			int id = item["id"];
			int quantity = item["quantity"];
			inventoryItems.AddItem(id, quantity);
		}
	}
}
