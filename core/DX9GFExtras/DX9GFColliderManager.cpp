#include "DX9GFColliderManager.h"

namespace DX9GF {

    void ColliderManager::Add(std::shared_ptr<ICollider> collider) {
        if (collider) {
            colliders.push_back(collider);
        }
    }

    void ColliderManager::Remove(std::shared_ptr<ICollider> collider) {
        auto it = std::find(colliders.begin(), colliders.end(), collider);
        if (it != colliders.end()) {
            colliders.erase(it);
        }
    }

    void ColliderManager::Clear() {
        colliders.clear();
    }

    std::optional<std::tuple<float, float>> ColliderManager::GetSafePosition(
        std::shared_ptr<ICollider> target,
        float newX,
        float newY)
    {
        if (!target) return std::nullopt;

        float finalX = newX;
        float finalY = newY;
        bool hasCollision = false;

        for (auto& other : colliders) {
            // Bỏ qua chính nó
            if (other == target) continue;

            // Kiểm tra va chạm và lấy vị trí đã được điều chỉnh (nếu có)
            auto result = target->IsIntersecting(other, finalX, finalY);
            if (result.has_value()) {
                auto& [correctedX, correctedY] = result.value();
                finalX = correctedX;
                finalY = correctedY;
                hasCollision = true;
            }
        }

        if (hasCollision) {
            return std::make_tuple(finalX, finalY);
        }

        return std::nullopt;
    }

    const std::vector<std::shared_ptr<ICollider>>& ColliderManager::GetAllColliders() const {
        return colliders;
    }

    std::tuple<float, float> DX9GF::ColliderManager::GetSlidingDeltas(std::shared_ptr<ICollider> target, float deltaX, float deltaY)
    {
        if (!target) return { deltaX, deltaY };

        float currentX = target->GetWorldX();
        float currentY = target->GetWorldY();

        float finalDx = deltaX;
        float finalDy = deltaY;

        for (auto& other : colliders) {
            if (other == target) continue;

            // Xử lý riêng trục X
            if (auto pos = target->IsIntersecting(other, currentX + finalDx, currentY); pos.has_value()) {
                auto& [correctedX, correctedY] = pos.value();
                finalDx = correctedX - currentX;
            }

            // Xử lý riêng trục Y
            if (auto pos = target->IsIntersecting(other, currentX, currentY + finalDy); pos.has_value()) {
                auto& [correctedX, correctedY] = pos.value();
                finalDy = correctedY - currentY;
            }
        }

        return std::make_tuple(finalDx, finalDy);
    }
}