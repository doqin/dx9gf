#include "pch.h"
#include "GameItems.h"
#include "LocalizationManager.h"
namespace Demo
{
	// Names and descriptions here reuse the exact same Tr() keys as IBattleScene::DrawModifierIcons
	// and IEnemy's status list wherever the wording is already instance-independent, so no new
	// translation is needed for those. Only Poison/Burn/Marked/Regen/EnergyDrain need a
	// number-free rephrasing, since their in-battle descriptions embed the live value/duration.
	ModifierInfo GetModifierInfo(ModifierType type)
	{
		switch (type) {
		case ModifierType::BuffDamage: return { Tr(L"Atk Up"), Tr(L"Increases attack damage.") };
		case ModifierType::BuffDefense: return { Tr(L"Def Up"), Tr(L"Blocks incoming damage.") };
		case ModifierType::Poison: return { Tr(L"Poison"), Tr(L"Deals damage equal to its remaining duration at the end of each turn.") };
		case ModifierType::Vulnerable: return { Tr(L"Vulnerable"), Tr(L"Takes 50% more damage from attacks.") };
		case ModifierType::Weak: return { Tr(L"Weak"), Tr(L"Deals 25% less damage with attacks.") };
		case ModifierType::Stun: return { Tr(L"Stun"), Tr(L"Cannot take action this turn.") };
		case ModifierType::Burn: return { Tr(L"Burn"), Tr(L"Deals damage at the end of each turn, ignoring block.") };
		case ModifierType::Marked: return { Tr(L"Marked"), Tr(L"Takes extra damage from every hit.") };
		case ModifierType::Regen: return { Tr(L"Regen"), Tr(L"Heals at the end of each turn.") };
		case ModifierType::Spark: return { Tr(L"Spark"), Tr(L"Accumulates stacks. Deals no damage until detonated.") };
		case ModifierType::Freeze: return { Tr(L"Freeze"), Tr(L"Player's movement speed is reduced.") };
		case ModifierType::Immunity: return { Tr(L"Immunity"), Tr(L"Blocks debuffs and tick damage.\nLoses 1 charge per block.") };
		case ModifierType::EnergyDrain: return { Tr(L"Energy Drain"), Tr(L"Reduces Energy gained at the start of your turn.") };
		case ModifierType::InvertedControls: return { Tr(L"Reversed Controls"), Tr(L"Movement is flipped: up<->down, left<->right.") };
		default: return { L"", L"" };
		}
	}

	std::wstring GetModifierStackingInfo(bool stacksValue)
	{
		if (stacksValue) {
			return Tr(L"Stacks: strength adds up, duration refreshes to the longer one.");
		}
		return Tr(L"Stacks: duration adds up, strength refreshes to the stronger one.");
	}

	//register an item here
	void ItemData::LoadData()
	{
		itemRegistry[0] = ConsumableItem(0, Tr(L"Lesser Heal"),
			Tr(L"Instant Heal 25HP"),
			{ CombatModifier{ ModifierType::HealHP, 0, 25.0f, true, 0 } },
			{ 0, 0, 23, 35 });

		itemRegistry[1] = ConsumableItem(1, Tr(L"Lesser Defense"),
			Tr(L"Buff 30 Defense for 3 turns"),
			{ CombatModifier{ ModifierType::BuffDefense, 3, 30.0f, true, 1 } },
			{ 24, 0, 47, 35 });

		itemRegistry[2] = ConsumableItem(2, Tr(L"Lesser Damage"),
			Tr(L"Buff 2 Damage for 3 turns"),
			{ CombatModifier{ ModifierType::BuffDamage, 3, 2.0f, true, 1 } },
			{ 48, 0, 71, 35 });

		itemRegistry[3] = ConsumableItem(3, Tr(L"Adrenaline Rush"),
			Tr(L"Buff 2 Damage for 6 turns & Instant Heal 15HP"),
			{
				CombatModifier{ ModifierType::BuffDamage, 6, 2.0f, true, 1 },
				CombatModifier{ ModifierType::HealHP, 0, 15.0f, true, 0 }
			},
			{ 72, 0, 95, 35 });

		itemRegistry[4] = ConsumableItem(4, Tr(L"Enguard"),
			Tr(L"Instant Heal 20HP & Buff 25 Defense for 4 turns"),
			{
				CombatModifier{ ModifierType::BuffDefense, 4, 25.0f, true, 1 },
				CombatModifier{ ModifierType::HealHP, 0, 20.0f, true, 0 }
			},
			{ 96, 0, 119, 35 });

		itemRegistry[5] = ConsumableItem(5, Tr(L"Mega Heal Elixir"),
			Tr(L"Instant Heal 50HP"),
			{ CombatModifier{ ModifierType::HealHP, 0, 50.0f, true, 0 } },
			{ 0, 36, 23, 71 });

		itemRegistry[6] = ConsumableItem(6, Tr(L"Iron Wall Shield"),
			Tr(L"Buff 50.0 Defense for 3 turns"),
			{ CombatModifier{ ModifierType::BuffDefense, 3, 50.0f, true, 1 } },
			{ 24, 36, 47, 71 });

		itemRegistry[7] = ConsumableItem(7, Tr(L"Berserker's Wrath"),
			Tr(L"Buff 10 Damage for 1 turn"),
			{ CombatModifier{ ModifierType::BuffDamage, 1, 10.0f, true, 1 } },
			{ 48, 36, 71, 71 });

		itemRegistry[8] = ConsumableItem(8, Tr(L"Paladin's Blessing"),
			Tr(L"Instant Heal 35HP & Buff 20 Defense and 3 Damage for 3 turns"),
			{
				CombatModifier{ ModifierType::HealHP, 0, 35.0f, true, 0 },
				CombatModifier{ ModifierType::BuffDefense, 3, 20.0f, true, 1 },
				CombatModifier{ ModifierType::BuffDamage, 3, 3.0f, true, 1 }
			},
			{ 72, 36, 95, 71 });

		itemRegistry[9] = ConsumableItem(9, Tr(L"Titan's Resolve"),
			Tr(L"Buff 50 Defense and 3 Damage for 3 turns"),
			{
				CombatModifier{ ModifierType::BuffDefense, 3, 50.0f, true, 1 },
				CombatModifier{ ModifierType::BuffDamage, 3, 3.0f, true, 1 }
			},
			{ 96, 36, 119, 71 });

		itemRegistry[10] = ConsumableItem(99, Tr(L"Rusty Key"), Tr(L"..."), {}, { 120, 0, 143, 35 });
		itemRegistry[11] = ConsumableItem(99, Tr(L"G(r)ayStone"), Tr(L"Has no practical use. Just a trophy for our winner."), {}, { 120, 36, 143, 71 });

		itemRegistry[12] = ConsumableItem(12, Tr(L"Authentication Token"),
			Tr(L"A one-time access credential, pulled from a terminal in the alley."),
			{}, { 144, 0, 167, 35 });


		//TODO: Update gear rects here. Default size is 12x12. Assets can be 9x9 max, but please import 12x12 rects

		//gearRegistry[0] = { 0, L"Titan Core", L"Grants 20 Max HP.",
		//	{ {7,7,19,19},
		//	{40,7,52,19},
		//	{73,7,85,19},
		//	{107,7,119,19},
		//	{139,7,151,19},
		//	{172,7,184,19},
		//	{206,7,218,19},
		//	{239,7,251,19} },
		//	GearType::Passive, GearEffect::AddMaxHP, 20, 0};

		gearRegistry[0] = { 0, Tr(L"Titan Core"), Tr(L"Grants 10 Max HP."), //red core
			{ {7,91,19,103} }, GearType::Passive, GearEffect::AddMaxHP, 10, 0 };

		gearRegistry[1] = { 1, Tr(L"Energy Cell"), Tr(L"Active: Restore 1 Energy.\nCooldown: 3 turns."), //energy cell
			{ {7, 77, 19, 89} }, GearType::Active, GearEffect::AddEnergy, 1, 3 };

		gearRegistry[2] = { 2, Tr(L"Data Extractor"), Tr(L"Active: Draw 2 card.\nCooldown: 2 turns."), //.rar
			{ {7, 21, 19, 33} }, GearType::Active, GearEffect::DrawCard, 2, 2 };

		gearRegistry[3] = { 3, Tr(L"Memory Locker"), Tr(L"Active: Retain 1 hand card.\nCooldown: 2 turns."), //lock
			{ {7, 35, 19, 47} }, GearType::Active, GearEffect::RetainCard, 1, 2 };

		gearRegistry[4] = { 4, Tr(L"Midas Chip"), Tr(L"Gain extra Gold on win."), //cpu chip
			{ {7, 49, 19, 61} }, GearType::Passive, GearEffect::GoldBoost, 20, 0 };

		gearRegistry[5] = { 5, Tr(L"Aegis Plating"), Tr(L"Start combat with 15 Armor in 3 turns."), //actully a white plate
			{ {7, 63, 19, 75} }, GearType::Passive, GearEffect::StartArmor, 15, 0 };
	}

	const ConsumableItem* ItemData::GetItemBlueprint(int id)
	{
		auto it = itemRegistry.find(id);
		if (it != itemRegistry.end())
		{
			return &(it->second);
		}
		return nullptr;
	}

	const GearBlueprint* ItemData::GetGearBlueprint(int id)
	{
		auto it = gearRegistry.find(id);
		if (it != gearRegistry.end())
		{
			return &(it->second);
		}
		return nullptr;
	}

	void ItemInventory::InitFixedInventory(int maxItemTypes)
	{
		slots.clear();
		slots.reserve(maxItemTypes);
		for (int i = 0; i < maxItemTypes; i++)
		{
			slots.push_back({ i, 0 });
		}
	}

	void ItemInventory::AddItem(int id, int amount)
	{
		if (id >= 0 && id < slots.size())
		{
			EnsureCapacity(id);
			slots[id].quantity += amount;
		}
	}

	bool ItemInventory::ConsumeItem(int id)
	{
		if (id >= 0 && id < slots.size() && slots[id].quantity > 0 && !IsItemLocked(id))
		{
			slots[id].quantity--;
			DX9GF::AudioManager::GetInstance()->PlayRandom("power_up", 0.2f);
			return true;
		}
		return false;
	}
	bool ItemInventory::RemoveItem(int id)
	{
		if (id >= 0 && id < slots.size() && slots[id].quantity > 0)
		{
			slots[id].quantity--;
			return true;
		}
		return false;
	}

	void ItemInventory::EnsureCapacity(int requiredID) {
		if (requiredID >= slots.size()) {
			int oldSize = slots.size();
			slots.resize(requiredID + 1);
			for (int i = oldSize; i <= requiredID; ++i) {
				slots[i] = { i, 0 };
			}
		}
	}
	bool ItemInventory::HasItem(int id) const {
		if (id >= 0 && id < slots.size()) {
			return slots[id].quantity > 0;
		}
		return false;
	}

	void ItemInventory::LockItem(int id, int turns)
	{
		if (turns <= 0) return;
		auto it = lockedItems.find(id);
		if (it == lockedItems.end() || it->second < turns) {
			lockedItems[id] = turns;
		}
	}

	bool ItemInventory::IsItemLocked(int id) const
	{
		return GetItemLockedTurns(id) > 0;
	}

	int ItemInventory::GetItemLockedTurns(int id) const
	{
		auto it = lockedItems.find(id);
		return (it != lockedItems.end()) ? it->second : 0;
	}

	void ItemInventory::TickLocks()
	{
		for (auto it = lockedItems.begin(); it != lockedItems.end(); ) {
			if (--(it->second) <= 0) it = lockedItems.erase(it);
			else ++it;
		}
	}
}
