#pragma once
#include "DX9GF.h"
#include <unordered_map>
namespace Demo
{

	// Registry key of the Authentication Token key item (see ItemData::LoadData).
	constexpr int ITEM_AUTH_TOKEN = 12;

	enum class GearType {
		Passive,
		Active
	};

	enum class GearEffect {
		AddMaxHP,
		AddEnergy,
		DrawCard,
		RetainCard,
		GoldBoost,
		StartArmor
	};

	struct GearBlueprint {
		int id;
		std::wstring name;
		std::wstring description;
		std::vector<RECT> frames;
		GearType type;
		GearEffect effect;
		int effectValue;
		int maxCooldownTurns;
	};

	enum class ModifierType {
		HealHP,
		BuffDamage,
		BuffDefense,
		Poison,
		Vulnerable,
		Weak,
		Stun,
		// Flat damage per turn that ignores block. Unlike Poison, its damage does not decay with
		// its remaining duration, and re-applying it accumulates value instead of replacing it.
		Burn,
		// Flat bonus to every point of incoming damage, applied before Vulnerable's multiplier.
		// The debuff mirror of BuffDamage.
		Marked,
		// Heals value at end of turn.
		Regen,
		//For ignite and destonate combo
		Spark,
		Freeze,
		Immunity,
		EnergyDrain,
		InvertedControls
		//...Add more if you have ideas
	};

	// A status effect a card applies, and how re-applying it on top of an existing instance
	// behaves - shown in the card's tooltip alongside GetModifierInfo's description. Matches
	// ICombatant::AddModifier (stacksValue=false: duration adds, value takes the max) vs
	// ::AddStackingModifier (stacksValue=true: value adds, duration takes the max).
	struct AppliedStatusEffect {
		ModifierType type;
		bool stacksValue;
	};

	struct ModifierInfo {
		std::wstring name;
		std::wstring description;
	};

	// General, instance-independent explanation of what a status effect does, for previewing a
	// card's effect before it is played. IBattleScene/IEnemy's own status tooltips describe an
	// already-applied instance instead, with its live value/duration embedded in the text.
	ModifierInfo GetModifierInfo(ModifierType type);

	// A short note on how re-applying this status effect while one is already active behaves.
	std::wstring GetModifierStackingInfo(bool stacksValue);

	struct CombatModifier {
		ModifierType type;
		int duration;
		float value;
		bool isBuff;
		// Grace turns: the modifier is active from the moment it is applied, but its duration does
		// not start counting down until these are burned off. Using an item costs a turn, so items
		// set this to 1 to stop that turn eating the buff they just handed out.
		int delayTurns = 0;
	};

	class ConsumableItem
	{
	private:
		int id;
		std::wstring name;
		std::wstring description;
		std::vector<CombatModifier> modifiers; 
		RECT itemRect;
	public:
		ConsumableItem(int _id = -1, std::wstring _name = L"NULL", std::wstring _desc = L"", std::vector<CombatModifier> _modifiers = {}, RECT _itemRect = RECT{ 0,0,0,0 })
			: id(_id), name(_name), description(_desc), modifiers(_modifiers), itemRect(_itemRect) {
		}

		int GetID() const { return id; }
		const std::wstring& GetName() const { return name; }
		const std::wstring& GetDescription() const { return description; }
		const std::vector<CombatModifier>& GetModifiers() const { return modifiers; }
		RECT GetItemRect() const { return itemRect; }
	};

	class ItemData
	{
	private:
		std::unordered_map<int, ConsumableItem> itemRegistry;
		std::unordered_map<int, GearBlueprint> gearRegistry;
		ItemData() {}
	public:
		static ItemData* GetInstance()
		{
			static ItemData instance;
			return &instance;
		}

		void LoadData();
		const ConsumableItem* GetItemBlueprint(int id);
		const GearBlueprint* GetGearBlueprint(int id);
	};

	struct ItemInventorySlot
	{
		int itemID;
		int quantity;
	};

	class ItemInventory
	{
	private:
		std::vector <ItemInventorySlot> slots;
		std::unordered_map<int, int> lockedItems;
	public:
		const std::vector<ItemInventorySlot>& GetSlots() const { return slots; }
		void InitFixedInventory(int maxItemTypes);
		void EnsureCapacity(int requiredID);
		bool HasItem(int id) const;
		void AddItem(int id, int amount);
		bool ConsumeItem(int id);
		// Removes one copy of the item silently (no "item used" sound). Used by the shop's sell
		// mode. Returns true if a copy was present and removed.
		bool RemoveItem(int id);
		void Clear() { for (auto& slot : slots) slot.quantity = 0; }
		void LockItem(int id, int turns);
		bool IsItemLocked(int id) const;
		int GetItemLockedTurns(int id) const;
		void TickLocks();
	};
}

