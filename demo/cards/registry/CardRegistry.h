#pragma once
#include "ICard.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Demo {
	class DraggableManager;

	// Which shop tiers stock a card. A card can be stocked by several.
	enum CardShopTier : unsigned {
		ShopNone = 0,
		ShopBasic = 1u << 0,
		ShopHybrid = 1u << 1,
		ShopPremium = 1u << 2,
	};

	struct CardDef {
		// Stored in saves. Never change it for an existing card.
		std::string id;
		std::function<std::shared_ptr<ICard>(std::weak_ptr<DX9GF::TransformManager>)> create;
		// Shop price. 0 means the card is not sold and is never rolled from a card pack.
		int price = 0;
		unsigned shopTiers = ShopNone;
	};

	// The single list of every card in the game. Adding a card means writing its class and adding
	// one line to the table in CardRegistry.cpp; creation from a save id, shop stock, pricing and
	// card packs all read from here.
	class CardRegistry {
	public:
		static const CardRegistry& Get();

		const CardDef* Find(const std::string& id) const;
		const std::vector<CardDef>& All() const { return defs; }

		// Builds the card for a save id, or returns null for an id that no longer exists (renamed or
		// removed card). When a draggable manager, device and camera are given, a draggable card is
		// also initialised so it can be placed in a scene.
		std::shared_ptr<ICard> Create(
			const std::string& id,
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::shared_ptr<DraggableManager> draggableManager = nullptr,
			DX9GF::GraphicsDevice* graphicsDevice = nullptr,
			DX9GF::Camera* camera = nullptr
		) const;

		// Cards a shop tier sells, in the order the shop lists them.
		std::vector<const CardDef*> ShopStock(CardShopTier tier) const;

	private:
		CardRegistry();
		std::vector<CardDef> defs;
		std::unordered_map<std::string, size_t> index;
	};
}
