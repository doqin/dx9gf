#include "pch.h"
#include "CardRegistry.h"
#include "IDraggable.h"
#include "AttackCards.h"
#include "DebuffCards.h"
#include "PoisonCards.h"
#include "MarkedCards.h"
#include "SparkCards.h"
#include "BurnCards.h"
#include "UtilityCards.h"
#include "MainBlockCard.h"

namespace {
	using namespace Demo;

	template <typename TCard>
	CardDef Def(const char* id, int price = 0, unsigned shopTiers = ShopNone) {
		std::string saveId = id;
		return {
			saveId,
			[saveId](std::weak_ptr<DX9GF::TransformManager> tm) -> std::shared_ptr<ICard> {
				auto card = std::make_shared<TCard>(tm);
				card->SetSaveID(saveId);
				return card;
			},
			price,
			shopTiers
		};
	}
}

Demo::CardRegistry::CardRegistry() {
	// Shop order within a tier is the order of this table. Price also sets rarity (CardCatalog).
	defs = {
		// Basic shop
		Def<StrikeCard>("StrikeCard", 30, ShopBasic),
		Def<HeavyStrikeCard>("HeavyStrikeCard", 70, ShopBasic),
		Def<PoisonCard>("PoisonCard", 50, ShopBasic),
		Def<EnergyCard>("EnergyCard", 45, ShopBasic),
		Def<JabCard>("JabCard", 20, ShopBasic),
		Def<BraceCard>("BraceCard", 25, ShopBasic),
		Def<MarkCard>("MarkCard", 30, ShopBasic),
		Def<RagingStrikeCard>("RagingStrikeCard", 45, ShopBasic),
		Def<ArmorPiercerCard>("ArmorPiercerCard", 50, ShopBasic),
		Def<LethalHarvestCard>("LethalHarvestCard", 55, ShopBasic),
		Def<ImmunityCard>("ImmunityCard", 50, ShopBasic),

		// Hybrid shop
		Def<TwinStrikeCard>("TwinStrikeCard", 40, ShopHybrid),
		Def<CleaveCard>("CleaveCard", 40, ShopHybrid | ShopPremium),
		Def<VulnerableCard>("VulnerableCard", 65, ShopHybrid),
		Def<WeaknessCard>("WeaknessCard", 85, ShopHybrid),
		Def<PrefetchCard>("PrefetchCard", 35, ShopHybrid),
		Def<OverclockCard>("OverclockCard", 35, ShopHybrid),
		Def<JumpstartCard>("JumpstartCard", 55, ShopHybrid),
		Def<ForesightCard>("ForesightCard", 70, ShopHybrid),
		// Ignite is the only source of Spark, so it and its detonator have to share a tier -
		// selling the payoff without the setup would put a dead card in the player's deck.
		Def<IgniteCard>("IgniteCard", 60, ShopHybrid),
		Def<FireDetonationCard>("FireDetonationCard", 90, ShopHybrid),
		Def<ShortCircuitCard>("ShortCircuitCard", 55, ShopHybrid),
		Def<StaticChargeCard>("StaticChargeCard", 95, ShopHybrid),
		Def<ChainDetonationCard>("ChainDetonationCard", 110, ShopHybrid),
		Def<ToxicCloudCard>("ToxicCloudCard", 70, ShopHybrid),
		Def<ContagionCard>("ContagionCard", 55, ShopHybrid),
		Def<SepticStrikeCard>("SepticStrikeCard", 60, ShopHybrid),
		Def<KindleCard>("KindleCard", 40, ShopHybrid),
		Def<FanTheFlamesCard>("FanTheFlamesCard", 65, ShopHybrid),
		Def<DragnetCard>("DragnetCard", 70, ShopHybrid),
		Def<BarrageCard>("BarrageCard", 85, ShopHybrid),
		// Cruel Strike doubles off Weak, which this tier already sells.
		Def<CruelStrikeCard>("CruelStrikeCard", 75, ShopHybrid),
		Def<ShieldBashCard>("ShieldBashCard", 70, ShopHybrid),

		// Premium shop (Cleave above is stocked here too)
		Def<ChainLightningCard>("ChainLightningCard", 100, ShopPremium),
		Def<InfernoCard>("InfernoCard", 140, ShopPremium),
		Def<TerminateCard>("TerminateCard", 150, ShopPremium),
		Def<OverdriveCard>("OverdriveCard", 180, ShopPremium),
		Def<SystemPurgeCard>("SystemPurgeCard", 200, ShopPremium),
		Def<SystemRestartCard>("SystemRestartCard", 175, ShopPremium),
		// These three only pay off against a built-out block - Overload scales with the
		// persistent cards installed in it, Chain Reaction with what resolved just before it,
		// and Execute wants the energy to spend in the first place.
		Def<ChainReactionCard>("ChainReactionCard", 120, ShopPremium),
		Def<ExecuteCard>("ExecuteCard", 160, ShopPremium),
		Def<OverloadCard>("OverloadCard", 170, ShopPremium),
		Def<FesteringCard>("FesteringCard", 130, ShopPremium),
		// Inferno, the only Burn source here before Kindle, is also Premium, so Meltdown sits with it.
		Def<MeltdownCard>("MeltdownCard", 120, ShopPremium),
		Def<HunterCard>("HunterCard", 110, ShopPremium),

		// Not sold or rolled
		Def<StunCard>("StunCard"),
		Def<MainBlockCard>("MainBlockCard"),
	};
	for (size_t i = 0; i < defs.size(); ++i) {
		index[defs[i].id] = i;
	}
}

const Demo::CardRegistry& Demo::CardRegistry::Get() {
	static const CardRegistry registry;
	return registry;
}

const Demo::CardDef* Demo::CardRegistry::Find(const std::string& id) const {
	auto it = index.find(id);
	return it != index.end() ? &defs[it->second] : nullptr;
}

std::shared_ptr<Demo::ICard> Demo::CardRegistry::Create(
	const std::string& id,
	std::weak_ptr<DX9GF::TransformManager> transformManager,
	std::shared_ptr<DraggableManager> draggableManager,
	DX9GF::GraphicsDevice* graphicsDevice,
	DX9GF::Camera* camera) const {
	const CardDef* def = Find(id);
	if (!def) return nullptr;
	auto card = def->create(transformManager);
	if (card && draggableManager && graphicsDevice && camera) {
		if (auto draggable = std::dynamic_pointer_cast<IDraggable>(card)) {
			draggable->Init(draggableManager, graphicsDevice, camera);
		}
	}
	return card;
}

std::vector<const Demo::CardDef*> Demo::CardRegistry::ShopStock(CardShopTier tier) const {
	std::vector<const CardDef*> stock;
	for (const auto& def : defs) {
		if (def.shopTiers & tier) stock.push_back(&def);
	}
	return stock;
}
