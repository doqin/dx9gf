#pragma once
#include "IShopScene.h"
#include "CardCatalog.h"
#include "CardRegistry.h"
#include "LocalizationManager.h"
#include <algorithm>

namespace Demo {
	class CardShop : public IShopScene {
	private:
		ShopTier currentTier;
		// Builds the listing from the card itself, so its description, face artwork and the card
		// the player actually receives can never drift apart. The price comes from CardCatalog,
		// shared with the card-pack peddler.
		static void ApplyCardFace(ShopItem& row, const ICard& card) {
			row.cardTemplate = card.GetCardTemplate();
			row.cardName = card.GetDisplayName();
			row.cardCost = card.GetCost();
			row.cardInputs = CardInputSignature(card.GetInputSlotCount(), 0);
		}
		void AddShopCard(const CardDef& def);
	public:
		CardShop(Game* game, Player* player, int screenWidth, int screenHeight, ShopTier tier,
			ShopLocation location = ShopLocation::Tutorial);


		void LoadItems() override;
		void LoadSellItems() override;
	};
}