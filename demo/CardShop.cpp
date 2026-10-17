#include "pch.h"
#include "CardShop.h"
#include "CardRegistry.h"
#include "ICard.h"
#include <cctype>
#include <map>

namespace {
	// "HeavyStrikeCard" -> "Heavy Strike Card". The buy row's label, which is also its translation key.
	std::string ShopLabel(const std::string& saveId)
	{
		std::string label;
		for (size_t i = 0; i < saveId.size(); ++i) {
			const unsigned char c = static_cast<unsigned char>(saveId[i]);
			if (i > 0 && std::isupper(c) && !std::isupper(static_cast<unsigned char>(saveId[i - 1]))) {
				label += ' ';
			}
			label += saveId[i];
		}
		return label;
	}

	// "HeavyStrikeCard" -> "Heavy Strike". Only used for the sell row's status message; the row
	// itself shows the card's face art, same as the buy rows.
	std::wstring PrettyCardName(const std::string& saveId)
	{
		std::string s = saveId;
		const std::string suffix = "Card";
		if (s.size() > suffix.size() &&
			s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0) {
			s.erase(s.size() - suffix.size());
		}
		std::wstring out;
		for (size_t i = 0; i < s.size(); ++i) {
			const unsigned char c = static_cast<unsigned char>(s[i]);
			if (i > 0 && std::isupper(c) && !std::isupper(static_cast<unsigned char>(s[i - 1]))) {
				out += L' ';
			}
			out += static_cast<wchar_t>(c);
		}
		return out;
	}
}

Demo::CardShop::CardShop(Game* game, Player* player, int sw, int sh, ShopTier tier)
	: IShopScene(game, player, sw, sh,
		tier == ShopTier::BASIC ? "BASIC CARD SHOP" :
		tier == ShopTier::HYBRID ? "HYBRID CARD SHOP" : "PREMIUM CARD SHOP"),
	currentTier(tier)
{
}

void Demo::CardShop::AddShopCard(const CardDef& def)
{
	auto prototype = def.create(this->transformManager);
	itemsForSale.push_back({
		Tr(DX9GF::Utils::Utf8ToWide(ShopLabel(def.id))),
		CardCatalog::GetPrice(def.id),
		prototype->GetDescription(),
		[this, id = def.id]() { this->player->AddCardToInventory(id); },
		RECT{ 0, 0, 0, 0 },
		ShopIconSheet::CardFaces,
		prototype->IsPersistent(),
		prototype->HasLimitedUses(),
		prototype->HasLimitedUses() ? prototype->GetMaxUses() : 0
		});
	ApplyCardFace(itemsForSale.back(), *prototype);
	itemsForSale.back().ownedCount = [this, id = def.id]() {
		const auto& inv = this->player->GetInventoryCards();
		const auto& deck = this->player->GetDeck();
		return static_cast<int>(std::count(inv.begin(), inv.end(), id) +
			std::count(deck.begin(), deck.end(), id));
		};
}

void Demo::CardShop::LoadItems()
{
	CardShopTier stockedTier = ShopNone;
	switch (currentTier) {
	case ShopTier::BASIC: stockedTier = ShopBasic; break;
	case ShopTier::HYBRID: stockedTier = ShopHybrid; break;
	case ShopTier::PREMIUM: stockedTier = ShopPremium; break;
	default: break;
	}
	for (const CardDef* def : CardRegistry::Get().ShopStock(stockedTier)) {
		AddShopCard(*def);
	}
}

void Demo::CardShop::LoadSellItems()
{
	// Only loose cards (the inventory, not the battle deck) can be sold. Cards are grouped by
	// type so a stack of three shows as one row with an "x3" count.
	std::map<std::string, int> counts;
	for (const auto& id : player->GetInventoryCards()) {
		counts[id]++;
	}

	for (const auto& [id, count] : counts) {
		// GetSellValue is a small fraction of the card's shop price, deliberately low so a card
		// pack (100G for 5) can never be milked for gold by dumping its contents here.
		const int sellValue = CardCatalog::GetSellValue(id);
		if (sellValue <= 0) continue; // starter cards and anything with no shop price

		auto proto = CardRegistry::Get().Create(id, transformManager);
		if (!proto) continue;

		const std::string sellId = id;
		ShopItem row;
		row.name = PrettyCardName(id);
		row.cost = sellValue;
		row.description = proto->GetDescription();
		row.iconSheet = ShopIconSheet::CardFaces;
		ApplyCardFace(row, *proto);
		row.isPersistent = proto->IsPersistent();
		row.hasLimitedUses = proto->HasLimitedUses();
		row.maxUses = proto->HasLimitedUses() ? proto->GetMaxUses() : 0;
		row.stackCount = count;
		row.onBuyAction = [this, sellId]() { this->player->RemoveCardFromInventory(sellId); };
		itemsToSell.push_back(std::move(row));
	}
}
