#pragma once
#include "TargetedCard.h"

namespace Demo {
	// Build-around cards for Marked: a flat bonus on every hit. Marked takes the strongest source
	// rather than stacking, so these mark in bulk and reward hitting many times.

	// Applied with AddModifier, not a stacking one: stacked values never decayed while their duration
	// kept being refreshed, so Marked climbed without limit and out-scaled every other deck.
	class MarkCard : public TargetedCard {
	public:
		MarkCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 0,
				.face = CardTemplate::Periwinkle,
				.description = L"Apply Marked 4 for 2 turns (target takes 4 extra damage per hit).",
				.persistent = false,
				.effects = { Effect::Apply(ModifierType::Marked, 2, 4.f) } }) {}
	};

	// Four hits, so every point of Marked is counted four times.
	class BarrageCard : public TargetedCard {
	public:
		BarrageCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 2,
				.face = CardTemplate::Periwinkle,
				.description = L"Deal 3 damage to an enemy 4 times.",
				.persistent = false,
				.effects = { Effect::Hit(3.f) },
				.repeat = 4 }) {}
	};

	// Has to beat Mark's 4 on a single target - most bosses are one - not just on the field. Twice
	// Mark's strength for 1 energy does, and the spread to every other enemy is the bonus on top.
	class DragnetCard : public IStatementCard {
	public:
		DragnetCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), IStatementCard(tm, {
				.cost = 1,
				.face = CardTemplate::Periwinkle,
				.description = L"Apply Marked 8 for 2 turns to all enemies.",
				.persistent = false,
				.effects = { Effect::ApplyAll(ModifierType::Marked, 2, 8.f) } }) {}
	};

	// Installed early, this keeps the target at a strong Marked for the whole cycle without
	// spending a card per turn on it.
	class HunterCard : public TargetedCard {
	public:
		HunterCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Periwinkle,
				.description = L"Apply Marked 6 for 2 turns to the target each turn.",
				.effects = { Effect::Apply(ModifierType::Marked, 2, 6.f) } }) {}
	};
}
