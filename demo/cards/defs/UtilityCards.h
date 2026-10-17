#pragma once
#include "IStatementCard.h"

namespace Demo {
	// Cards that act on the player or the battle rather than on a target: resources, buffs and
	// utility. Every card with a 0 cost is non-persistent, which is what pays for the zero cost: a
	// card left in the program re-executes free on every turn of the cycle, so a 0-cost persistent
	// card would be unlimited free value. These fire once per placement and have to be drawn and
	// placed again to fire again.

	// Spend 1 energy now to start the next turn with one extra.
	class EnergyCard : public IStatementCard {
	public:
		EnergyCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Cyan,
				.description = L"Gain 1 extra energy next turn." }) {}

		void Resolve() override;
	};

	class BraceCard : public IStatementCard {
	public:
		BraceCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 0,
				.face = CardTemplate::Steel,
				.description = L"Gain 5 block for 2 turns.",
				.persistent = false,
				// Stacking, so two Brace cards in one program give 10 block rather than 5.
				.effects = { Effect::Gain(ModifierType::BuffDefense, 2, 5.f) } }) {}

		void Resolve() override;
	};

	// Instant draw. In the init block the new cards arrive in time to be played this turn; in the
	// main block they arrive as the turn ends and are discarded with the rest of the hand.
	class PrefetchCard : public IStatementCard {
	public:
		PrefetchCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 0,
				.face = CardTemplate::Blue,
				.description = L"Draw 2 cards now.",
				.persistent = false,
				.maxUses = 2 }) {}

		void Resolve() override;
	};

	class OverclockCard : public IStatementCard {
	public:
		OverclockCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 0,
				.face = CardTemplate::Cyan,
				.description = L"Gain 1 energy now. Take 4 damage.",
				.persistent = false,
				.maxUses = 2 }) {}

		void Resolve() override;
	};

	// The 1-cost pair that contrasts the two flavours of resource. Jumpstart pays out now and is
	// only worth playing from the init block; Foresight pays out next turn and, being persistent,
	// keeps paying out every turn until the program clears.
	class JumpstartCard : public IStatementCard {
	public:
		JumpstartCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Cyan,
				.description = L"Gain 2 energy now.",
				.persistent = false }) {}

		void Resolve() override;
	};

	class ForesightCard : public IStatementCard {
	public:
		ForesightCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Blue,
				.description = L"Draw 1 extra card next turn." }) {}

		void Resolve() override;
	};

	class ImmunityCard : public IStatementCard {
	public:
		ImmunityCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Aqua,
				.description = L"Apply Immunity 3. Blocks 1 incoming Debuff or Tick Damage per charge.",
				.persistent = false,
				.effects = { Effect::Gain(ModifierType::Immunity, 3, 3.f) } }) {}
	};

	// A legendary utility card that clears all debuffs from the player.
	class SystemRestartCard : public IStatementCard {
	public:
		SystemRestartCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Sage,
				.description = L"Clear all debuffs and tick damage from yourself.",
				.persistent = false,
				.maxUses = 1 }) {}

		void Resolve() override;
	};

	class OverdriveCard : public IStatementCard {
	private:
		static constexpr int TURNS = 3;
		static constexpr float ATTACK_BUFF = 2.f;
		static constexpr float REGEN_BUFF = 6.f;
	public:
		OverdriveCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 3,
				.face = CardTemplate::Chartreuse,
				.persistent = false,
				.effects = {
					Effect::Gain(ModifierType::BuffDamage, TURNS, ATTACK_BUFF),
					Effect::Gain(ModifierType::Regen, TURNS, REGEN_BUFF) } }) {}

		std::wstring GetDescription() const override;
		void Resolve() override;
	};

	class SystemPurgeCard : public IStatementCard {
	public:
		SystemPurgeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 4,
				.face = CardTemplate::Pine,
				.description = L"Deal 16 damage to all enemies and stun them for 1 turn.",
				.persistent = false,
				.effects = { Effect::HitAll(16.f), Effect::ApplyAll(ModifierType::Stun, 1) } }) {}

		void Resolve() override;
	};
}
