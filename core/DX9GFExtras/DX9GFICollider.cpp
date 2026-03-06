#include "DX9GFICollider.h"
#include "DX9GFEllipseCollider.h"
#include "DX9GFRectangleCollider.h"
#include <algorithm>
#include <cmath>

namespace {
    constexpr float collisionEpsilon = 0.001f;

    float Clamp(float value, float min, float max)
    {
        return (std::max)(min, (std::min)(value, max));
    }

    bool IsRectEllipseOverlapping(
        float rectX,
        float rectY,
        float rectWidth,
        float rectHeight,
        float ellipseCenterX,
        float ellipseCenterY,
        float ellipseRadiusX,
        float ellipseRadiusY
    )
    {
        float closestX = Clamp(ellipseCenterX, rectX, rectX + rectWidth);
        float closestY = Clamp(ellipseCenterY, rectY, rectY + rectHeight);
        float dx = (ellipseCenterX - closestX) / ellipseRadiusX;
        float dy = (ellipseCenterY - closestY) / ellipseRadiusY;
        return dx * dx + dy * dy < 1.0f;
    }

    std::tuple<float, float> ResolveInitialOverlap(
        float rectX,
        float rectY,
        float rectWidth,
        float rectHeight,
        float ellipseCenterX,
        float ellipseCenterY,
        float ellipseRadiusX,
        float ellipseRadiusY
    )
    {
        float closestX = Clamp(ellipseCenterX, rectX, rectX + rectWidth);
        float closestY = Clamp(ellipseCenterY, rectY, rectY + rectHeight);
        float vx = closestX - ellipseCenterX;
        float vy = closestY - ellipseCenterY;
        float normalizedDistance = std::sqrt(
            (vx * vx) / (ellipseRadiusX * ellipseRadiusX)
            + (vy * vy) / (ellipseRadiusY * ellipseRadiusY)
        );

        float correctionX = 0.0f;
        float correctionY = 0.0f;

        if (normalizedDistance > 0.0f) {
            float scale = (1.0f - normalizedDistance) / normalizedDistance;
            correctionX = vx * scale;
            correctionY = vy * scale;
        }
        else {
            float pushLeft = (ellipseCenterX - ellipseRadiusX) - (rectX + rectWidth);
            float pushRight = (ellipseCenterX + ellipseRadiusX) - rectX;
            float pushUp = (ellipseCenterY - ellipseRadiusY) - (rectY + rectHeight);
            float pushDown = (ellipseCenterY + ellipseRadiusY) - rectY;

            correctionX = pushLeft;
            float shortest = std::abs(pushLeft);

            if (std::abs(pushRight) < shortest) {
                correctionX = pushRight;
                shortest = std::abs(pushRight);
            }

            if (std::abs(pushUp) < shortest) {
                correctionX = 0.0f;
                correctionY = pushUp;
                shortest = std::abs(pushUp);
            }

            if (std::abs(pushDown) < shortest) {
                correctionX = 0.0f;
                correctionY = pushDown;
            }
        }

        if (correctionX != 0.0f) {
            correctionX += correctionX > 0.0f ? collisionEpsilon : -collisionEpsilon;
        }
        if (correctionY != 0.0f) {
            correctionY += correctionY > 0.0f ? collisionEpsilon : -collisionEpsilon;
        }

        return std::make_tuple(rectX + correctionX, rectY + correctionY);
    }
}

std::optional<std::tuple<float, float>> DX9GF::ICollider::IsIntersecting(std::weak_ptr<RectangleCollider> targetCollider, std::weak_ptr<EllipseCollider> otherCollider, float newX, float newY)
{
    auto target = targetCollider.lock();
    auto other = otherCollider.lock();
    if (!target || !other) {
        return std::nullopt;
    }

    float currentX = target->GetWorldX();
    float currentY = target->GetWorldY();
    float targetWidth = target->GetWidth();
    float targetHeight = target->GetHeight();

    float otherWidth = other->GetWidth();
    float otherHeight = other->GetHeight();
    float ellipseRadiusX = otherWidth / 2.0f;
    float ellipseRadiusY = otherHeight / 2.0f;
    if (targetWidth <= 0.0f || targetHeight <= 0.0f || ellipseRadiusX <= 0.0f || ellipseRadiusY <= 0.0f) {
        return std::nullopt;
    }

    float ellipseCenterX = other->GetWorldX() + ellipseRadiusX;
    float ellipseCenterY = other->GetWorldY() + ellipseRadiusY;

    bool isCurrentlyOverlapping = IsRectEllipseOverlapping(
        currentX,
        currentY,
        targetWidth,
        targetHeight,
        ellipseCenterX,
        ellipseCenterY,
        ellipseRadiusX,
        ellipseRadiusY
    );

    if (isCurrentlyOverlapping) {
        bool isOverlappingAtNewPosition = IsRectEllipseOverlapping(
            newX,
            newY,
            targetWidth,
            targetHeight,
            ellipseCenterX,
            ellipseCenterY,
            ellipseRadiusX,
            ellipseRadiusY
        );

        if (!isOverlappingAtNewPosition) {
            return std::nullopt;
        }

        return ResolveInitialOverlap(
            newX,
            newY,
            targetWidth,
            targetHeight,
            ellipseCenterX,
            ellipseCenterY,
            ellipseRadiusX,
            ellipseRadiusY
        );
    }

    float deltaX = newX - currentX;
    float deltaY = newY - currentY;
    if (deltaX == 0.0f && deltaY == 0.0f) {
        return std::nullopt;
    }

    float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    float minimumFeature = (std::min)((std::min)(targetWidth, targetHeight), (std::min)(ellipseRadiusX, ellipseRadiusY));
    int steps = 16;
    if (minimumFeature > 0.0f) {
        steps = (std::max)(steps, static_cast<int>(std::ceil((distance / minimumFeature) * 4.0f)));
    }
    steps = (std::min)(steps, 512);

    float previousT = 0.0f;
    for (int i = 1; i <= steps; i++) {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        float testX = currentX + deltaX * t;
        float testY = currentY + deltaY * t;
        if (IsRectEllipseOverlapping(
            testX,
            testY,
            targetWidth,
            targetHeight,
            ellipseCenterX,
            ellipseCenterY,
            ellipseRadiusX,
            ellipseRadiusY
        )) {
            float low = previousT;
            float high = t;
            for (int j = 0; j < 12; j++) {
                float mid = (low + high) / 2.0f;
                float midX = currentX + deltaX * mid;
                float midY = currentY + deltaY * mid;
                if (IsRectEllipseOverlapping(
                    midX,
                    midY,
                    targetWidth,
                    targetHeight,
                    ellipseCenterX,
                    ellipseCenterY,
                    ellipseRadiusX,
                    ellipseRadiusY
                )) {
                    high = mid;
                }
                else {
                    low = mid;
                }
            }

            float correctedT = (std::max)(0.0f, high - collisionEpsilon);
            return std::make_tuple(currentX + deltaX * correctedT, currentY + deltaY * correctedT);
        }

        previousT = t;
    }

    return std::nullopt;
}

std::optional<std::tuple<float, float>> DX9GF::ICollider::IsIntersecting(std::weak_ptr<EllipseCollider> targetCollider, std::weak_ptr<RectangleCollider> otherCollider, float newX, float newY)
{
    auto target = targetCollider.lock();
    auto other = otherCollider.lock();
    if (!target || !other) {
        return std::nullopt;
    }

    float targetCurrentX = target->GetWorldX();
    float targetCurrentY = target->GetWorldY();
    float otherCurrentX = other->GetWorldX();
    float otherCurrentY = other->GetWorldY();

    float deltaX = newX - targetCurrentX;
    float deltaY = newY - targetCurrentY;

    auto correctedOtherPosition = IsIntersecting(
        otherCollider,
        targetCollider,
        otherCurrentX - deltaX,
        otherCurrentY - deltaY
    );

    if (!correctedOtherPosition.has_value()) {
        return std::nullopt;
    }

    auto& [correctedOtherX, correctedOtherY] = correctedOtherPosition.value();
    return std::make_tuple(
        targetCurrentX + (otherCurrentX - correctedOtherX),
        targetCurrentY + (otherCurrentY - correctedOtherY)
    );
}
