#pragma once
#include "IShopScene.h"
#include <unordered_map>

namespace Demo {
    class ItemShop : public IShopScene {
    private:
        ShopTier currentTier;
        ShopLocation location;
        // Rows already listed per item, so a potion stocked twice gets two separate purchase counters.
        std::unordered_map<int, int> rowsPerItem;
        void AddShopItem(int itemID, int price, bool limited = true);
    public:
        ItemShop(Game* game, Player* player, int screenWidth, int screenHeight, ShopTier tier,
            ShopLocation location = ShopLocation::Tutorial);
        void LoadItems() override;
        void LoadSellItems() override;
    };
}