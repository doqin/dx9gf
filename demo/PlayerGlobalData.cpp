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
	decks = { { "Deck 1", deck } };
	activeDeckIndex = 0;
	inventoryCards.clear();
	inventoryItems = ItemInventory();
	inventoryItems.InitFixedInventory(13);
	inventoryGears.clear();
	pendingGearPopups.clear();
	equippedActiveGearID = -1;
	equippedPassiveGearID = -1;
	showGearOnMap = true;
	seenBattleTutorial = false;
	unlockedSavePoints.clear();
}

void Demo::PlayerGlobalData::SyncActiveDeck() {
	if (activeDeckIndex >= 0 && activeDeckIndex < (int)decks.size()) decks[activeDeckIndex].cards = deck;
}

void Demo::PlayerGlobalData::SwitchDeck(int index) {
	if (index < 0 || index >= (int)decks.size() || index == activeDeckIndex) return;
	SyncActiveDeck();
	activeDeckIndex = index;
	deck = decks[index].cards;
}

void Demo::PlayerGlobalData::CreateDeck() {
	SyncActiveDeck();
	// Pick the first "Deck N" not already in use.
	for (int n = (int)decks.size() + 1;; n++) {
		std::string name = "Deck " + std::to_string(n);
		bool taken = false;
		for (auto& d : decks) taken = taken || d.name == name;
		if (!taken) { decks.push_back({ name, {} }); break; }
	}
	SwitchDeck((int)decks.size() - 1);
}

bool Demo::PlayerGlobalData::DeleteDeck(int index) {
	if (decks.size() <= 1 || index < 0 || index >= (int)decks.size()) return false;
	SyncActiveDeck();
	for (auto& card : decks[index].cards) inventoryCards.push_back(card);
	decks.erase(decks.begin() + index);
	if (activeDeckIndex > index) activeDeckIndex--;
	else if (activeDeckIndex == index) activeDeckIndex = (std::min)(index, (int)decks.size() - 1);
	deck = decks[activeDeckIndex].cards;
	return true;
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
	outData["unlockedSavePoints"] = unlockedSavePoints;

	outData["inventoryGears"] = nlohmann::json::array();
	for (int gear : inventoryGears) {
		outData["inventoryGears"].push_back(gear);
	}

	outData["deck"] = nlohmann::json::array();
	for (auto& card : deck) {
		outData["deck"].push_back(card);
	}
	// "deck" above stays the active deck so older readers still work.
	SyncActiveDeck();
	outData["activeDeckIndex"] = activeDeckIndex;
	outData["decks"] = nlohmann::json::array();
	for (auto& d : decks) {
		outData["decks"].push_back({ { "name", d.name }, { "cards", d.cards } });
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
	unlockedSavePoints.clear();
	if (inData.contains("unlockedSavePoints")) {
		unlockedSavePoints = inData["unlockedSavePoints"].get<std::vector<std::string>>();
	}

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
	// Old saves have no "decks": the single "deck" becomes "Deck 1".
	decks.clear();
	if (inData.contains("decks")) {
		for (auto& d : inData["decks"]) {
			decks.push_back({ d.value("name", std::string("Deck")), d["cards"].get<std::vector<std::string>>() });
		}
	}
	if (decks.empty()) decks.push_back({ "Deck 1", deck });
	activeDeckIndex = inData.value("activeDeckIndex", 0);
	if (activeDeckIndex < 0 || activeDeckIndex >= (int)decks.size()) activeDeckIndex = 0;
	deck = decks[activeDeckIndex].cards;
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
