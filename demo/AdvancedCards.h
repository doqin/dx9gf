#pragma once
#include "MultiTargetCard.h"

namespace Demo {

	// OFFENSIVE CARDS

	class HeavyStrikeCard : public MultiTargetCard {
	public:
		HeavyStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Heavy Strike", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			if (!targets.empty()) {
				if (auto enemy = targets[0].lock()) {
					if (auto e = enemy->GetValue()) e->TakeDamage(12.f);
				}
			}
			isDone = true;
			return true;
		}
	};

	class TwinStrikeCard : public MultiTargetCard {
	private:
		int hits = 0;
	public:
		TwinStrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Twin Strike", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			if (!targets.empty()) {
				if (auto enemy = targets[0].lock()) {
					if (auto e = enemy->GetValue()) e->TakeDamage(3.f);
					hits++;
				}
			}
			if (hits >= 2 || targets.empty()) {
				isDone = true;
				return true;
			}
			return false;
		}
		void ResetExecution() override {
			MultiTargetCard::ResetExecution();
			hits = 0;
		}
	};

	class CleaveCard : public MultiTargetCard {
	public:
		CleaveCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 2, L"Cleave", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			for (auto& wp : targets) {
				if (auto enemy = wp.lock()) {
					if (auto e = enemy->GetValue()) e->TakeDamage(4.f);
				}
			}
			isDone = true;
			return true;
		}
	};

	class ChainLightningCard : public MultiTargetCard {
	public:
		ChainLightningCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 3, L"Chain L.", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			float damages[] = { 6.f, 4.f, 2.f };
			for (size_t i = 0; i < targets.size() && i < 3; ++i) {
				if (auto enemy = targets[i].lock()) {
					if (auto e = enemy->GetValue()) e->TakeDamage(damages[i]);
				}
			}
			isDone = true;
			return true;
		}
	};

	// EFFECT CARDS

	class PoisonCard : public MultiTargetCard {
	public:
		PoisonCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Poison", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			if (!targets.empty()) {
				if (auto enemy = targets[0].lock()) {
					if (auto e = enemy->GetValue()) e->ApplyStatus(StatusType::POISON, 3, 2.f);
				}
			}
			isDone = true;
			return true;
		}
	};

	class VulnerableCard : public MultiTargetCard {
	public:
		VulnerableCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Vulnerable", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			if (!targets.empty()) {
				if (auto enemy = targets[0].lock()) {
					if (auto e = enemy->GetValue()) e->ApplyStatus(StatusType::VULNERABLE, 2);
				}
			}
			isDone = true;
			return true;
		}
	};

	class WeaknessCard : public MultiTargetCard {
	public:
		WeaknessCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 2, L"Weakness", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			for (auto& wp : targets) {
				if (auto enemy = wp.lock()) {
					if (auto e = enemy->GetValue()) e->ApplyStatus(StatusType::WEAK, 2);
				}
			}
			isDone = true;
			return true;
		}
	};

	class StunCard : public MultiTargetCard {
	public:
		StunCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0)
			: IGameObject(tm, x, y), MultiTargetCard(tm, 1, L"Stun", x, y) {
		}
		bool Execute() override {
			if (isDone) return true;
			if (!targets.empty()) {
				if (auto enemy = targets[0].lock()) {
					if (auto e = enemy->GetValue()) e->ApplyStatus(StatusType::STUN, 1);
				}
			}
			isDone = true;
			return true;
		}
	};
}