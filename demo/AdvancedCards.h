#pragma once
#include "MultiTargetCard.h"
#include "LocalizationManager.h"

namespace Demo {

	// OFFENSIVE CARDS

	class HeavyStrikeCard : public MultiTargetCard {
	public:
		HeavyStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Heavy Strike", x, y, 192, 32) {
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 16 damage to an enemy."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 16.f, 1); }

	};

	class TwinStrikeCard : public MultiTargetCard {
	private:
		int hits = 0;
	public:
		TwinStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Twin Strike", x, y, 192, 32) {
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 3 damage to an enemy twice."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		// Two passes over the same target - Execute lands 3 damage twice.
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectHitsOnTargets(state, 3.f, 1);
			CollectHitsOnTargets(state, 3.f, 1);
		}


		void ResetExecution() override;
	};

	class CleaveCard : public MultiTargetCard {
	public:
		CleaveCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 2, L"Cleave", x, y, 160, 32) {
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 7 damage to up to 2 enemies."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 7.f); }

	};

	class ChainLightningCard : public MultiTargetCard {
		const float baseDamage = 20.f;
		const float damageReductionPerRepeat = 75.f;
	public:
		ChainLightningCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 3, L"Chain L.", x, y, 224, 32) {
		}

		size_t GetCost() const override { return 3; }
		std::wstring GetDescription() const override {
			return Tr(L"Deal ") + std::to_wstring(static_cast<int>(baseDamage)) + Tr(L" damage to up to 3 enemies, but base damage is reduced by ")
				+ std::to_wstring(static_cast<int>(damageReductionPerRepeat)) + Tr(L"% \neach time it targets the same enemy.");
		}
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;

	};

	// EFFECT CARDS

	class PoisonCard : public MultiTargetCard {
		const int poisonTurns = 3;
	public:
		PoisonCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Poison", x, y, 160, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Poison ") + std::to_wstring(poisonTurns) + Tr(L" (damage equals remaining turns)."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Poison, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Green; }

		bool Execute() override;
		// No value of its own, so its tick is worth however many turns are on it - and AddModifier
		// adds to whatever is already there, so poisoning an already-poisoned enemy ticks harder.
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Poison, 0.f, poisonTurns, 1);
		}

	};

	class VulnerableCard : public MultiTargetCard {
	public:
		VulnerableCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Vulnerable", x, y, 192, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Vulnerable 1 to an enemy."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Vulnerable, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Magenta; }

		bool Execute() override;
		// Vulnerable has no value of its own - it is a flat 1.5x on everything queued after it.
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Vulnerable, 0.f, 1, 1);
		}

	};

	class WeaknessCard : public MultiTargetCard {
	public:
		WeaknessCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Weakness", x, y, 192, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Weak 2 to an enemy."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Weak, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Rose; }

		bool Execute() override;

		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Weak, 0.f, 2, 2);
		}
	};

	class StunCard : public MultiTargetCard {
	public:
		StunCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Stun", x, y, 160, 32) {
		}

		size_t GetCost() const override { return 3; }
		std::wstring GetDescription() const override { return Tr(L"Stun an enemy for 1 turn."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Stun, false } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Forest; }

		bool Execute() override;

		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Stun, 0.f, 1, 1);
		}
	};

	class IgniteCard : public MultiTargetCard {
	public:
		IgniteCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Ignite", x, y, 160, 32) {
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Apply Spark 2 for 3 turns."); }
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Spark, true } }; }

		CardTemplate GetCardTemplate() const override { return CardTemplate::Yellow; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectEffectOnTargets(state, ModifierType::Spark, 2.f, 3, 1);
		}
	};

	class FireDetonationCard : public MultiTargetCard {
	public:
		FireDetonationCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Fire Deton.", x, y, 224, 32) {
			SetPersistent(false);
		}

		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 3 damage. Consumes all Spark on target to deal 5 extra damage per stack."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Yellow; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;

	};

	class RagingStrikeCard : public MultiTargetCard {
	private:
		int currentDamage = 4;
		bool hasExecutedThisCycle = false;
	public:
		RagingStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Raging Strike", x, y, 192, 32) {
		}

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override {
			return Tr(L"Deal ") + std::to_wstring(currentDamage) + Tr(L" damage. Increases this card's damage by 3 when played. Resets to 4 damage on discard.");
		}
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }

		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override {
			CollectHitsOnTargets(state, static_cast<float>(currentDamage), 1);
		}
		void OnDiscard() override {
			currentDamage = 4;
		}
	};
	class OverloadCard : public MultiTargetCard {
	public:
		OverloadCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Overload", 0, 0, 160, 32) {
			SetPersistent(false);
			//SetMaxUses(1);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 5 DMG. Deal +4 DMG for each Persistent card currently in the Block."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class ChainReactionCard : public MultiTargetCard {
	public:
		ChainReactionCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Chain Reaction", 0, 0, 192, 32) {
			SetPersistent(true);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 5 DMG. If the previous card killed its target, deal 8 DMG to the lowest HP enemy instead."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class LethalHarvestCard : public MultiTargetCard {
	public:
		LethalHarvestCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Lethal Harvest", 0, 0, 192, 32) {
			SetPersistent(true);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 5.f, 1); }
		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 5 DMG. If fatal, heal 8 HP."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class ExecuteCard : public MultiTargetCard {
	public:
		ExecuteCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Execute", 0, 0, 160, 32) {
			SetPersistent(false);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override { CollectHitsOnTargets(state, 15.f, 1); }
		size_t GetCost() const override { return 2; }
		std::wstring GetDescription() const override { return Tr(L"Deal 15 DMG. If fatal, gain 1 Energy next turn."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class ArmorPiercerCard : public MultiTargetCard {
	public:
		ArmorPiercerCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Armor Piercer", 0, 0, 192, 32) {
			SetPersistent(true);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 5 DMG. Deals 2x damage to enemies with Armor. Ignores their defense."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class CruelStrikeCard : public MultiTargetCard {
	public:
		CruelStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Cruel Strike", 0, 0, 192, 32) {
			SetPersistent(false);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Deal 8 DMG. Deals 2x damage if target is Weak."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class ShieldBashCard : public MultiTargetCard {
	public:
		ShieldBashCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), MultiTargetCard(tm, 1, L"Shield Bash", 0, 0, 160, 32) {
			SetPersistent(false);
		}
		bool Execute() override;
		void CollectProjectedSteps(VirtualBattleState& state) override;
		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override { return Tr(L"Consume all your Armor. Deal damage equal to 1.5x the consumed amount."); }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Red; }
	};

	class ImmunityCard : public IStatementCard {
	private:
		bool isDone = false;
	public:
		ImmunityCard(std::weak_ptr<DX9GF::TransformManager> tm)
			: IGameObject(tm, 0, 0), IStatementCard(tm, 160, 32, 0, 0) {
			SetPersistent(false);
		}

		bool Execute() override;
		void ResetExecution() override;

		size_t GetCost() const override { return 1; }
		std::wstring GetDescription() const override {
			return Tr(L"Apply Immunity 3. Blocks 1 incoming Debuff or Tick Damage per charge.");
		}
		std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const override { return { { ModifierType::Immunity, true } }; }
		CardTemplate GetCardTemplate() const override { return CardTemplate::Aqua; }
	};
}