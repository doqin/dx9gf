#include "pch.h"
#include "UtilityCards.h"
#include "IBattleScene.h"
#include "LocalizationManager.h"

void Demo::EnergyCard::Resolve() {
	if (!battleScene) return;
	battleScene->QueueBonusEnergy(1);
	battleScene->QueuePopUpMessage(L"+1 energy next turn");
}

void Demo::BraceCard::Resolve() {
	ApplyEffects();
	if (owner && battleScene) battleScene->QueuePopUpMessage(Tr(L"+5 block"));
}

void Demo::PrefetchCard::Resolve() {
	if (!battleScene) return;
	battleScene->DrawCardsNow(2);
	battleScene->QueuePopUpMessage(Tr(L"Drew 2 cards"));
}

void Demo::OverclockCard::Resolve() {
	if (battleScene) {
		battleScene->GainEnergyNow(1);
		battleScene->QueuePopUpMessage(Tr(L"+1 energy"));
	}
	if (owner) owner->TakeIndirectDamage(4.f, DamageType::Physical);
}

void Demo::JumpstartCard::Resolve() {
	if (!battleScene) return;
	battleScene->GainEnergyNow(2);
	battleScene->QueuePopUpMessage(Tr(L"+2 energy"));
}

void Demo::ForesightCard::Resolve() {
	if (!battleScene) return;
	battleScene->QueueBonusDraw(1);
	battleScene->QueuePopUpMessage(Tr(L"+1 card next turn"));
}

void Demo::SystemRestartCard::Resolve() {
	if (!owner) return;
	bool hasDebuff = false;
	for (const auto& mod : owner->GetModifiers()) {
		if (!mod.isBuff && mod.duration > 0) {
			hasDebuff = true;
			break;
		}
	}

	if (hasDebuff) {
		owner->ClearDebuffs();
		if (battleScene) {
			battleScene->QueuePopUpMessage(Tr(L"Debuffs Cleared!"));
			DX9GF::AudioManager::GetInstance()->PlayRandom("power_up", 0.6f);
		}
	}
	else if (battleScene) {
		battleScene->QueuePopUpMessage(Tr(L"No debuffs to clear"));
	}
}

std::wstring Demo::OverdriveCard::GetDescription() const {
	// "over", not "for" - "for" is already Tr()'d elsewhere (IShopScene's "Sold X for YG")
	// with a price-context translation that would be wrong reused here.
	return Tr(L"Gain ") + std::to_wstring(static_cast<int>(ATTACK_BUFF)) + Tr(L" attack and Regen ")
		+ std::to_wstring(static_cast<int>(REGEN_BUFF)) + Tr(L" over ") + std::to_wstring(TURNS) + Tr(L" turns.");
}

void Demo::OverdriveCard::Resolve() {
	if (!owner) return;
	ApplyEffects();
	if (battleScene) battleScene->QueuePopUpMessage(Tr(L"Overdrive!"));
}

void Demo::SystemPurgeCard::Resolve() {
	if (!battleScene || !owner) return;
	ApplyEffects();
	battleScene->QueuePopUpMessage(Tr(L"System purged"));
}
