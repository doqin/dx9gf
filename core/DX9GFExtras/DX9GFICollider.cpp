#include "DX9GFICollider.h"
#include "DX9GFEllipseCollider.h"
#include "DX9GFRectangleCollider.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace {
    constexpr float collisionEpsilon = 0.001f;
    constexpr float floatEpsilon = 0.000001f;
    constexpr float pi = 3.14159265358979323846f;
    constexpr int overlapSamples = 48;

    struct Vec2 {
        float x;
        float y;
    };

    Vec2 operator+(const Vec2& a, const Vec2& b) { return { a.x + b.x, a.y + b.y }; }
    Vec2 operator-(const Vec2& a, const Vec2& b) { return { a.x - b.x, a.y - b.y }; }
    Vec2 operator*(const Vec2& v, float s) { return { v.x * s, v.y * s }; }

    float Dot(const Vec2& a, const Vec2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    float LengthSq(const Vec2& v)
    {
        return v.x * v.x + v.y * v.y;
    }

    Vec2 Normalize(const Vec2& v)
    {
        float lenSq = LengthSq(v);
        if (lenSq <= floatEpsilon) {
            return { 1.0f, 0.0f };
        }
        float invLen = 1.0f / std::sqrt(lenSq);
        return { v.x * invLen, v.y * invLen };
    }

    Vec2 Rotate(const Vec2& v, float radians)
    {
        float c = std::cos(radians);
        float s = std::sin(radians);
        return { v.x * c - v.y * s, v.x * s + v.y * c };
    }

    Vec2 GetCenter(const std::array<Vec2, 4>& corners)
    {
        return (corners[0] + corners[1] + corners[2] + corners[3]) * 0.25f;
    }

    std::array<Vec2, 4> BuildRectangleCorners(DX9GF::RectangleCollider& collider, float originWorldX, float originWorldY)
    {
        float r = collider.GetWorldRotation();
        float sx = collider.GetWorldScaleX();
        float sy = collider.GetWorldScaleY();
        float ox = collider.GetOriginX();
        float oy = collider.GetOriginY();

        std::array<Vec2, 4> local = {
            Vec2{ 0.0f, 0.0f },
            Vec2{ collider.GetWidth(), 0.0f },
            Vec2{ collider.GetWidth(), collider.GetHeight() },
            Vec2{ 0.0f, collider.GetHeight() }
        };

        std::array<Vec2, 4> world;
        for (size_t i = 0; i < local.size(); i++) {
            Vec2 p = { (local[i].x - ox) * sx, (local[i].y - oy) * sy };
            p = Rotate(p, r);
            world[i] = { originWorldX + p.x, originWorldY + p.y };
        }
        return world;
    }

    bool PointInOrientedRectangle(const std::array<Vec2, 4>& corners, const Vec2& p)
    {
        Vec2 axisX = corners[1] - corners[0];
        Vec2 axisY = corners[3] - corners[0];
        float lenX = std::sqrt((std::max)(floatEpsilon, LengthSq(axisX)));
        float lenY = std::sqrt((std::max)(floatEpsilon, LengthSq(axisY)));
        axisX = axisX * (1.0f / lenX);
        axisY = axisY * (1.0f / lenY);

        Vec2 center = GetCenter(corners);
        Vec2 d = p - center;
        float px = std::abs(Dot(d, axisX));
        float py = std::abs(Dot(d, axisY));
        return px <= (lenX * 0.5f + floatEpsilon) && py <= (lenY * 0.5f + floatEpsilon);
    }

    Vec2 GetEllipseCenter(DX9GF::EllipseCollider& collider, float originWorldX, float originWorldY)
    {
        float rx = collider.GetWidth() * 0.5f;
        float ry = collider.GetHeight() * 0.5f;
        float r = collider.GetWorldRotation();
        float sx = collider.GetWorldScaleX();
        float sy = collider.GetWorldScaleY();
        float ox = collider.GetOriginX();
        float oy = collider.GetOriginY();
        Vec2 centerOffset = Rotate({ (rx - ox) * sx, (ry - oy) * sy }, r);
        return { originWorldX + centerOffset.x, originWorldY + centerOffset.y };
    }

    Vec2 TransformEllipseLocalPointToWorld(DX9GF::EllipseCollider& collider, float originWorldX, float originWorldY, const Vec2& localPoint)
    {
        float r = collider.GetWorldRotation();
        float sx = collider.GetWorldScaleX();
        float sy = collider.GetWorldScaleY();
        float ox = collider.GetOriginX();
        float oy = collider.GetOriginY();
        Vec2 p = Rotate({ (localPoint.x - ox) * sx, (localPoint.y - oy) * sy }, r);
        return { originWorldX + p.x, originWorldY + p.y };
    }

    bool EllipseContainsPoint(DX9GF::EllipseCollider& collider, float originWorldX, float originWorldY, const Vec2& worldPoint)
    {
        float rx = collider.GetWidth() * 0.5f;
        float ry = collider.GetHeight() * 0.5f;
        if (rx <= floatEpsilon || ry <= floatEpsilon) {
            return false;
        }

        float r = collider.GetWorldRotation();
        float sx = collider.GetWorldScaleX();
        float sy = collider.GetWorldScaleY();
        if (std::abs(sx) <= floatEpsilon || std::abs(sy) <= floatEpsilon) {
            return false;
        }

        Vec2 centerWorld = GetEllipseCenter(collider, originWorldX, originWorldY);
        Vec2 d = worldPoint - centerWorld;
        Vec2 localScaled = Rotate(d, -r);
        Vec2 local = { localScaled.x / sx, localScaled.y / sy };

        float nx = local.x / rx;
        float ny = local.y / ry;
        return (nx * nx + ny * ny) <= (1.0f + floatEpsilon);
    }

    bool IsRectEllipseOverlappingTransformed(
        DX9GF::RectangleCollider& rect,
        float rectOriginX,
        float rectOriginY,
        DX9GF::EllipseCollider& ellipse,
        float ellipseOriginX,
        float ellipseOriginY
    )
    {
        auto rectCorners = BuildRectangleCorners(rect, rectOriginX, rectOriginY);
        for (const auto& corner : rectCorners) {
            if (EllipseContainsPoint(ellipse, ellipseOriginX, ellipseOriginY, corner)) {
                return true;
            }
        }

        float rx = ellipse.GetWidth() * 0.5f;
        float ry = ellipse.GetHeight() * 0.5f;
        if (rx <= floatEpsilon || ry <= floatEpsilon) {
            return false;
        }

        for (int i = 0; i < overlapSamples; i++) {
            float t = static_cast<float>(i) / static_cast<float>(overlapSamples);
            float theta = t * 2.0f * pi;
            Vec2 localPoint = { rx + std::cos(theta) * rx, ry + std::sin(theta) * ry };
            Vec2 worldPoint = TransformEllipseLocalPointToWorld(ellipse, ellipseOriginX, ellipseOriginY, localPoint);
            if (PointInOrientedRectangle(rectCorners, worldPoint)) {
                return true;
            }
        }

        return false;
    }

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
    float otherX = other->GetWorldX();
    float otherY = other->GetWorldY();

    auto IsOverlappingAt = [&](float testX, float testY) {
        return IsRectEllipseOverlappingTransformed(*target, testX, testY, *other, otherX, otherY);
    };

    bool isCurrentlyOverlapping = IsOverlappingAt(currentX, currentY);
    if (isCurrentlyOverlapping) {
        bool isOverlappingAtNewPosition = IsOverlappingAt(newX, newY);
        if (!isOverlappingAtNewPosition) {
            return std::nullopt;
        }

        auto rectCorners = BuildRectangleCorners(*target, newX, newY);
        Vec2 rectCenter = GetCenter(rectCorners);
        Vec2 ellipseCenter = GetEllipseCenter(*other, otherX, otherY);
        Vec2 dir = rectCenter - ellipseCenter;
        if (LengthSq(dir) <= floatEpsilon) {
            dir = { newX - currentX, newY - currentY };
        }
        dir = Normalize(dir);

        float rectFeature = (std::max)(target->GetWidth() * std::abs(target->GetWorldScaleX()), target->GetHeight() * std::abs(target->GetWorldScaleY()));
        float ellipseFeature = (std::max)(other->GetWidth() * std::abs(other->GetWorldScaleX()), other->GetHeight() * std::abs(other->GetWorldScaleY()));
        float high = (std::max)(10.0f, (rectFeature + ellipseFeature) * 0.5f + 10.0f);
        for (int tries = 0; tries < 6 && IsOverlappingAt(newX + dir.x * high, newY + dir.y * high); tries++) {
            high *= 2.0f;
        }
        if (IsOverlappingAt(newX + dir.x * high, newY + dir.y * high)) {
            return std::nullopt;
        }

        float low = 0.0f;
        for (int i = 0; i < 16; i++) {
            float mid = (low + high) * 0.5f;
            if (IsOverlappingAt(newX + dir.x * mid, newY + dir.y * mid)) {
                low = mid;
            }
            else {
                high = mid;
            }
        }

        return std::make_tuple(newX + dir.x * (high + collisionEpsilon), newY + dir.y * (high + collisionEpsilon));
    }

    float dx = newX - currentX;
    float dy = newY - currentY;
    if (dx == 0.0f && dy == 0.0f) {
        return std::nullopt;
    }

    float distance = std::sqrt(dx * dx + dy * dy);
    float rectMin = (std::min)(target->GetWidth() * std::abs(target->GetWorldScaleX()), target->GetHeight() * std::abs(target->GetWorldScaleY()));
    float ellipseMin = (std::min)(other->GetWidth() * std::abs(other->GetWorldScaleX()), other->GetHeight() * std::abs(other->GetWorldScaleY())) * 0.5f;
    float minimumFeature = (std::min)(rectMin, ellipseMin);
    int steps = 16;
    if (minimumFeature > 0.0f) {
        steps = (std::max)(steps, static_cast<int>(std::ceil((distance / minimumFeature) * 4.0f)));
    }
    steps = (std::min)(steps, 512);

    float previousT = 0.0f;
    for (int i = 1; i <= steps; i++) {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        float testX = currentX + dx * t;
        float testY = currentY + dy * t;
        if (IsOverlappingAt(testX, testY)) {
            float low = previousT;
            float high = t;
            for (int j = 0; j < 12; j++) {
                float mid = (low + high) * 0.5f;
                float midX = currentX + dx * mid;
                float midY = currentY + dy * mid;
                if (IsOverlappingAt(midX, midY)) {
                    high = mid;
                }
                else {
                    low = mid;
                }
            }

            float correctedT = (std::max)(0.0f, high - collisionEpsilon);
            return std::make_tuple(currentX + dx * correctedT, currentY + dy * correctedT);
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

bool DX9GF::ICollider::drawCollider = false;