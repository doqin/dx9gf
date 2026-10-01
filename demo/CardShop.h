#pragma once
#include "IShopScene.h"
#include "CardCatalog.h"
#include "LocalizationManager.h"

namespace Demo {
	class CardShop : public IShopScene {
 private:
		ShopTier currentTier;
		// Builds the listing from the card type itself, so its description, face artwork and
		// the card the player actually receives can never drift apart. The price comes from
		// CardCatalog, the single source of truth shared with the card-pack peddler.
		static void ApplyCardFace(ShopItem& row, const ICard& card) {
			row.cardTemplate = card.GetCardTemplate();
			row.cardName = card.GetDisplayName();
			row.cardCost = card.GetCost();
		}
		template <typename TCard>
		void AddShopCard(const std::string& name) {
			TCard prototype(this->transformManager);
			itemsForSale.push_back({
				Tr(DX9GF::Utils::Utf8ToWide(name)),
				CardCatalog::GetPrice(prototype.GetSaveID()),
				prototype.GetDescription(),
				[this]() {
					auto newCard = std::make_shared<TCard>(this->transformManager);
					this->player->AddCardToInventory(newCard->GetSaveID());
				},
				RECT{ 0, 0, 0, 0 },
				ShopIconSheet::CardFaces,
				prototype.IsPersistent(),
				prototype.HasLimitedUses(),
				prototype.HasLimitedUses() ? prototype.GetMaxUses() : 0
				});
			ApplyCardFace(itemsForSale.back(), prototype);
		}
	public:
        CardShop(Game* game, Player* player, int screenWidth, int screenHeight, ShopTier tier);


		void LoadItems() override;
		void LoadSellItems() override;
	};
}