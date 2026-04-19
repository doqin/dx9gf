#include "pch.h"
#include "GameItems.h"
namespace Demo
{
	//register an item here
	void ItemData::LoadData()
	{
		itemRegistry[0] = ConsumableItem(0, L"Instant Heal 20HP",
			{ ActiveBuff{ ItemBuffType::HealHP, 20.0f, 0 } },
			{ 0, 108, 23, 143 });

		itemRegistry[1] = ConsumableItem(1, L"Buff 10% Defense",
			{ ActiveBuff{ ItemBuffType::BuffDefense, 10.0f, 1 } },
			{ 0, 72, 23, 107 });

		itemRegistry[2] = ConsumableItem(2, L"Buff 15% Damage",
			{ ActiveBuff{ ItemBuffType::BuffDamage, 15.0f, 2 } },
			{ 72, 0, 95, 35 });

		itemRegistry[3] = ConsumableItem(3, L"Buff 8% Damage & Heal 15HP",
			{
				ActiveBuff{ ItemBuffType::BuffDamage, 8.0f, 3 },
				ActiveBuff{ ItemBuffType::HealHP, 15.0f, 0 }
			},
			{ 48, 36, 71, 71 });
		itemRegistry[4] = ConsumableItem(4, L"Buff 10% Defense & Heal 12HP",
			{
				ActiveBuff{ ItemBuffType::BuffDefense, 10.0f, 4 },
				ActiveBuff{ ItemBuffType::HealHP, 12.0f, 0 }
			},
			{ 72, 36, 95, 71 });

		itemRegistry[5] = ConsumableItem(5, L"Mega Heal Elixir",
			{ ActiveBuff{ ItemBuffType::HealHP, 45.0f, 0 } },
			{ 48, 0, 71, 35 });

		itemRegistry[6] = ConsumableItem(6, L"Iron Wall Shield",
			{ ActiveBuff{ ItemBuffType::BuffDefense, 40.0f, 1 } },
			{ 0, 0, 23, 35 });

		itemRegistry[7] = ConsumableItem(7, L"Berserker's Wrath",
			{ ActiveBuff{ ItemBuffType::BuffDamage, 35.0f, 1 } },
			{ 24, 0, 47, 35 });

		itemRegistry[8] = ConsumableItem(8, L"Paladin's Blessing",
			{
				ActiveBuff{ ItemBuffType::HealHP, 10.0f, 0 },
				ActiveBuff{ ItemBuffType::BuffDefense, 12.0f, 2 },
				ActiveBuff{ ItemBuffType::BuffDamage, 12.0f, 2 }
			},
			{ 288, 108, 311, 143 });

		itemRegistry[9] = ConsumableItem(9, L"Titan's Resolve",
			{
				ActiveBuff{ ItemBuffType::BuffDefense, 25.0f, 3 },
				ActiveBuff{ ItemBuffType::BuffDamage, 5.0f, 3 }
			},
			{ 264, 0, 287, 35 });

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
			slots[id].quantity += amount;
		}
	}

	bool ItemInventory::ConsumeItem(int id)
	{
		if (id >= 0 && id < slots.size() && slots[id].quantity > 0)
		{
			slots[id].quantity--;
			return true;
		}
		return false;
	}
}
