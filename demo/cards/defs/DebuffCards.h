#pragma once
#include "TargetedCard.h"

namespace Demo {
	class VulnerableCard : public TargetedCard {
	public:
		VulnerableCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Magenta,
				.description = L"Apply Vulnerable 1 to an enemy.",
				.persistent = false,
				.effects = { Effect::Apply(ModifierType::Vulnerable, 1) } }) {}
	};

	class WeaknessCard : public TargetedCard {
	public:
		WeaknessCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 1,
				.face = CardTemplate::Rose,
				.description = L"Apply Weak 2 to an enemy.",
				.persistent = false,
				.effects = { Effect::Apply(ModifierType::Weak, 2) } }) {}
	};

	// Not sold in any shop.
	class StunCard : public TargetedCard {
	public:
		StunCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm), TargetedCard(tm, 1, {
				.cost = 3,
				.face = CardTemplate::Forest,
				.description = L"Stun an enemy for 1 turn.",
				.effects = { Effect::Apply(ModifierType::Stun, 1) } }) {}
	};
}
