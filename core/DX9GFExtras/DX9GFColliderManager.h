#pragma once
#include "DX9GFICollider.h"
#include <vector>
#include <memory>
#include <optional>
#include <tuple>
#include <algorithm>

namespace DX9GF {
    class ColliderManager {
    private:
        std::vector<std::shared_ptr<ICollider>> colliders;

    public:
        ColliderManager() = default;
        ~ColliderManager() = default;

        void Add(std::shared_ptr<ICollider> collider);
        void Remove(std::shared_ptr<ICollider> collider);
        void Clear();
        std::optional<std::tuple<float, float>> GetSafePosition(
            std::shared_ptr<ICollider> target,
            float newX,
            float newY
        );
        const std::vector<std::shared_ptr<ICollider>>& GetAllColliders() const;
        std::tuple<float, float> GetSlidingDeltas(std::shared_ptr<ICollider> target, float deltaX, float deltaY);
    };
}