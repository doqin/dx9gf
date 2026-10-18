#include "pch.h"
#include "ItemShop.h"
#include "GameItems.h"
#include "ItemCatalog.h"

void Demo::ItemShop::AddShopItem(int itemID, int price, bool limited)
{
	const auto* blueprint = ItemData::GetInstance()->GetItemBlueprint(itemID);

	if (blueprint) {
		itemsForSale.push_back({
			blueprint->GetName(),
			price,
			blueprint->GetDescription(),
			[this, itemID]() {
				this->player->GetInventoryItems().AddItem(itemID, 1);
			},
			blueprint->GetItemRect(),
			ShopIconSheet::Items
			});
		itemsForSale.back().ownedCount = [this, itemID]() {
			for (const auto& slot : this->player->GetInventoryItems().GetSlots()) {
				if (slot.itemID == itemID) return slot.quantity;
			}
			return 0;
			};
		if (limited) {
			const int row = rowsPerItem[itemID]++;
			itemsForSale.back().limitKey = shopKey + "/" + std::to_string(itemID) + "#" + std::to_string(row);
			itemsForSale.back().purchaseLimit = ItemPurchaseLimit(location);
		}
	}
}

Demo::ItemShop::ItemShop(Game* game, Player* player, int sw, int sh, ShopTier tier, ShopLocation location)
	: IShopScene(game, player, sw, sh,
		tier == ShopTier::BASIC ? "BASIC SHOP" :
		tier == ShopTier::HYBRID || tier == ShopTier::RK_HYBRID ? "HYBRID SHOP" : "PREMIUM SHOP"),
	currentTier(tier),
	location(location)
{
	shopKey = MakeShopKey(location, "Item", tier);
}

void Demo::ItemShop::LoadItems()
{
	rowsPerItem.clear();
	//i set 100G to test, remember to balance them later
	switch (currentTier) {
	case ShopTier::BASIC:
		AddShopItem(0, 30);
		AddShopItem(1, 30);
		AddShopItem(2, 40);
		// Tutorial stocks 2 two-effect potions here. Thread Alley has two regular item shops,
		// so its 5 extra two-effect potions are split: 2 here, 3 in its hybrid shop.
		if (location == ShopLocation::Tutorial || location == ShopLocation::ThreadAlley) {
			AddShopItem(3, 50);
			AddShopItem(4, 50);
		}
		break;

	case ShopTier::HYBRID:
		AddShopItem(3, 50);
		AddShopItem(4, 50);
		AddShopItem(5, 50);
		// 5 extra two-effect potions. Secret Puzzle has only this item shop, so it gets all 5;
		// Thread Alley gets 3 here and the other 2 in its basic shop.
		AddShopItem(3, 50);
		AddShopItem(4, 50);
		AddShopItem(9, 150);
		if (location != ShopLocation::ThreadAlley) {
			AddShopItem(3, 50);
			AddShopItem(4, 50);
		}
		break;

	case ShopTier::PREMIUM:
		AddShopItem(6, 100);
		AddShopItem(7, 100);
		AddShopItem(8, 200);
		AddShopItem(9, 150);
		// 7 extra two- and three-effect potions. Boss World's other item shop (RK_HYBRID) is
		// the special Rusty Key shop, so all 7 go here. Three-effect potions are only sold here.
		AddShopItem(3, 50);
		AddShopItem(4, 50);
		AddShopItem(9, 150);
		AddShopItem(8, 200);
		AddShopItem(3, 50);
		AddShopItem(4, 50);
		AddShopItem(8, 200);
		break;
	case ShopTier::RK_HYBRID:
		//load hybrid items just like HYBRID shop
		AddShopItem(3, 50);
		AddShopItem(4, 50);
		AddShopItem(5, 50);
		//add that rusty key with high cost - balance this later
		AddShopItem(10, 100, false);
		break;
	}
}

void Demo::ItemShop::LoadSellItems()
{
	// Any battle consumable the player is carrying can be sold back at ItemCatalog::SELL_RATE of
	// its shop value. Key items and trophies return 0 from GetSellValue and are skipped.
	for (const auto& slot : player->GetInventoryItems().GetSlots()) {
		if (slot.quantity <= 0) continue;

		const int sellValue = ItemCatalog::GetSellValue(slot.itemID);
		if (sellValue <= 0) continue;

		const auto* blueprint = ItemData::GetInstance()->GetItemBlueprint(slot.itemID);
		if (!blueprint) continue;

		const int id = slot.itemID;
		ShopItem row;
		row.name = blueprint->GetName();
		row.cost = sellValue;
		row.description = blueprint->GetDescription();
		row.iconRect = blueprint->GetItemRect();
		row.iconSheet = ShopIconSheet::Items;
		row.stackCount = slot.quantity;
		row.onBuyAction = [this, id]() { this->player->GetInventoryItems().RemoveItem(id); };
		itemsToSell.push_back(std::move(row));
	}
}