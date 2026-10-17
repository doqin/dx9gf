#include "pch.h"
#include "TargetedCard.h"
#include "LocalizationManager.h"
#include "DrawUtils.h"
#include "IBattleScene.h"
#include "VirtualBattleState.h"

namespace Demo {
	TargetedCard::TargetedCard(std::weak_ptr<DX9GF::TransformManager> tm, size_t maxTargets, CardSpec cardSpec)
		: IGameObject(tm), IStatementCard(tm, std::move(cardSpec)), maxTargets(maxTargets) {
	}

	bool TargetedCard::OnDrop(std::shared_ptr<IDraggable> other) {
		if (targets.size() >= maxTargets) return false;

		auto incomingEnemyCard = std::dynamic_pointer_cast<EnemyCard>(other);
		if (!incomingEnemyCard) return false;

		auto [thisX, thisY] = this->GetWorldPosition();
		auto [otherX, otherY] = other->GetWorldPosition();
		otherY += other->GetTrigger().lock()->GetHeight() / 2.0f;

		if (otherX > thisX && otherX < thisX + GetWidth() &&
			otherY > thisY && otherY < thisY + GetHeight()) {
			return AttachEnemyCard(incomingEnemyCard);
		}
		return false;
	}

	bool TargetedCard::AttachEnemyCard(std::shared_ptr<EnemyCard> card) {
		if (!card || targets.size() >= maxTargets || !IsQueuedInBlock()) return false;

		float localX = (float)GetWidth();
		for (auto& wp : targets) {
			if (auto lock = wp.lock()) localX += lock->GetWidth();
		}

		card->SetParent(shared_from_this());
		card->SetLocalPosition(localX, 0);
		targets.push_back(card);
		return true;
	}

	std::shared_ptr<IEnemy> TargetedCard::LiveTarget() const {
		if (targets.empty()) return nullptr;
		auto card = targets[0].lock();
		if (!card || !card->GetValue() || card->GetValue()->IsDead()) return nullptr;
		return card->GetValue();
	}

	std::vector<std::shared_ptr<IEnemy>> TargetedCard::GetTargetEnemies() const {
		std::vector<std::shared_ptr<IEnemy>> enemies;
		for (const auto& wp : targets) {
			if (auto card = wp.lock()) {
				if (auto enemy = card->GetValue()) enemies.push_back(enemy);
			}
		}
		return enemies;
	}

	bool TargetedCard::HasEffectTarget() const {
		for (const auto& enemy : GetTargetEnemies()) {
			if (!enemy->IsDead()) return true;
		}
		return false;
	}

	void TargetedCard::CollectHitsOnTargets(VirtualBattleState& state, float damage, size_t maxHits) {
		size_t hits = 0;
		for (const auto& enemy : GetTargetEnemies()) {
			if (maxHits > 0 && hits >= maxHits) break;
			state.SimulateDamage(enemy.get(), damage);
			++hits;
		}
	}

	void TargetedCard::CollectEffectOnTargets(VirtualBattleState& state, ModifierType modifier, float value, int duration, size_t maxTargetsHit) {
		size_t applied = 0;
		for (const auto& enemy : GetTargetEnemies()) {
			if (maxTargetsHit > 0 && applied >= maxTargetsHit) break;
			state.SimulateEnemyModifier(enemy.get(), modifier, value, duration);
			++applied;
		}
	}

	void TargetedCard::ReleaseEnemyCards() {
		for (auto& wp : targets) {
			if (auto lock = wp.lock()) {
				if (battleScene) {
					battleScene->DiscardEnemyCard(lock);
				}
			}
		}
		targets.clear();
	}

	std::tuple<float, float> TargetedCard::GetEnemyCardSlotWorldPosition() const {
		float localX = (float)dragAreaWidth;
		for (auto& wp : targets) {
			if (auto lock = wp.lock()) localX += lock->GetWidth();
		}
		return { GetWorldX() + localX, GetWorldY() };
	}

	void TargetedCard::Update(unsigned long long deltaTime) {
		IDraggable::Update(deltaTime);
		bool needsReposition = false;

		for (auto it = targets.begin(); it != targets.end(); ) {
			bool remove = true;
			if (auto lock = it->lock()) {
				if (auto parent = lock->GetParent(); parent.has_value()) {
					if (parent.value().lock().get() == this) {
						remove = false;
					}
				}
			}
			if (remove) {
				it = targets.erase(it);
				needsReposition = true;
			}
			else {
				++it;
			}
		}

		if (needsReposition) {
			float localX = dragAreaWidth;
			for (auto& wp : targets) {
				if (auto lock = wp.lock()) {
					lock->SetLocalPosition(localX, 0);
					localX += lock->GetWidth();
				}
			}
		}
	}

	void TargetedCard::Draw(unsigned long long deltaTime) {
		IStatementCard::Draw(deltaTime);

		// Highlight the card while a lone enemy card is being dragged and could still be dropped here.
		const auto dragging = draggableManager->GetDraggingDraggables();
		if (dragging.size() != 1) return;
		auto draggedEnemyCard = std::dynamic_pointer_cast<EnemyCard>(dragging[0]);
		if (!draggedEnemyCard || !CanAcceptEnemyCard()) return;
		for (const auto& wp : targets) {
			if (wp.lock() == draggedEnemyCard) return;
		}

		const float width = (float)GetWidth();
		const float height = (float)GetHeight();
		const float thisX = GetWorldX();
		const float thisY = GetWorldY();
		// Captures the device and camera rather than `this`: the command runs when the manager
		// flushes its draw buffer, by which time the card may be gone.
		draggableManager->QueueDraw(std::make_shared<DX9GF::CustomCommand>(
			[gd = graphicsDevice, cam = camera, width, height, thisX, thisY](std::function<void(void)> markFinished) {
				gd->SetAlphaBlending(true);
				Demo::DrawAnimatedDashedRectangle(gd, *cam, thisX, thisY, width, height,
					3.f, 0xFFFFFFFF, false, 4.f, 0xFFFFFFFF, 20.f, 10.f, 40.f, GetTickCount64());
				gd->SetAlphaBlending(false);
				markFinished();
			}));
	}

	size_t TargetedCard::GetFilledInputCount() const {
		size_t filled = 0;
		for (const auto& wp : targets) if (!wp.expired()) ++filled;
		return filled;
	}

	std::wstring TargetedCard::GetInputsDescription() const {
		return std::to_wstring(targets.size()) + L"/" + std::to_wstring(maxTargets) + L" " + Tr(maxTargets == 1 ? L"Enemy" : L"Enemies");
	}
}
