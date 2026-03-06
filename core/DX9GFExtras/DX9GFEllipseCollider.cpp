#include "DX9GFEllipseCollider.h"
#include "DX9GFRectangleCollider.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
    constexpr float collisionEpsilon = 0.001f;
    constexpr float floatEpsilon = 0.000001f;

    bool IsEllipseEllipseOverlapping(
        float centerX,
        float centerY,
        float otherCenterX,
        float otherCenterY,
        float sumA,
        float sumB
    )
    {
        float dx = centerX - otherCenterX;
        float dy = centerY - otherCenterY;
        float normalizedDistanceSq = (dx * dx) / (sumA * sumA) + (dy * dy) / (sumB * sumB);
        return normalizedDistanceSq < (1.0f - floatEpsilon);
    }

    std::tuple<float, float> ResolveEllipseOverlap(
        float centerX,
        float centerY,
        float otherCenterX,
        float otherCenterY,
        float sumA,
        float sumB,
        float moveX,
        float moveY
    )
    {
        float px = (centerX - otherCenterX) / sumA;
        float py = (centerY - otherCenterY) / sumB;
        float length = std::sqrt(px * px + py * py);

        if (length <= floatEpsilon) {
            px = moveX / sumA;
            py = moveY / sumB;
            length = std::sqrt(px * px + py * py);
        }

        if (length <= floatEpsilon) {
            px = 1.0f;
            py = 0.0f;
            length = 1.0f;
        }

        float ux = px / length;
        float uy = py / length;

        float boundaryX = ux * sumA;
        float boundaryY = uy * sumB;

        float worldLength = std::sqrt(boundaryX * boundaryX + boundaryY * boundaryY);
        float epsilonX = 0.0f;
        float epsilonY = 0.0f;
        if (worldLength > floatEpsilon) {
            epsilonX = (boundaryX / worldLength) * collisionEpsilon;
            epsilonY = (boundaryY / worldLength) * collisionEpsilon;
        }

        return std::make_tuple(
            otherCenterX + boundaryX + epsilonX,
            otherCenterY + boundaryY + epsilonY
        );
    }
}

bool DX9GF::EllipseCollider::IsCollidedEllipse(std::weak_ptr<DX9GF::EllipseCollider> other) {
    auto lock = other.lock();
    if (!lock) {
        throw std::runtime_error("lock failed");
    }

    float a = width / 2;
    float b = height / 2;
    float otherA = lock->width / 2;
    float otherB = lock->height / 2;

    if (a <= 0 || b <= 0 || otherA <= 0 || otherB <= 0) {
        return false;
    }

    float centerX = GetWorldX() + a;
    float centerY = GetWorldY() + b;
    float otherCenterX = lock->GetWorldX() + otherA;
    float otherCenterY = lock->GetWorldY() + otherB;

    float dx = centerX - otherCenterX;
    float dy = centerY - otherCenterY;
    float sumA = a + otherA;
    float sumB = b + otherB;

    return (dx * dx) / (sumA * sumA) + (dy * dy) / (sumB * sumB) <= (1.0f + floatEpsilon);
}

bool DX9GF::EllipseCollider::IsCollided(std::weak_ptr<ICollider> other)
{
    auto lock = other.lock();
    if (auto ellipseCollider = dynamic_pointer_cast<EllipseCollider>(lock)) {
        return IsCollidedEllipse(ellipseCollider);
    }
    else {
        throw std::runtime_error("Unknown collider, method has not implemented logic");
    }
}

float DX9GF::EllipseCollider::GetWidth() const
{
    return width;
}

float DX9GF::EllipseCollider::GetHeight() const
{
    return height;
}

std::optional<std::tuple<float, float>> DX9GF::EllipseCollider::IsIntersecting(std::weak_ptr<ICollider> other, float newX, float newY)
{
    auto lock = other.lock();
    if (auto ellipseCollider = dynamic_pointer_cast<EllipseCollider>(lock)) {
        float currentX = GetWorldX();
        float currentY = GetWorldY();

        float a = width / 2;
        float b = height / 2;
        float otherA = ellipseCollider->width / 2;
        float otherB = ellipseCollider->height / 2;

        if (a <= 0 || b <= 0 || otherA <= 0 || otherB <= 0) {
            return std::nullopt;
        }

        float centerX = currentX + a;
        float centerY = currentY + b;
        float otherCenterX = ellipseCollider->GetWorldX() + otherA;
        float otherCenterY = ellipseCollider->GetWorldY() + otherB;

        float sumA = a + otherA;
        float sumB = b + otherB;
        bool isCurrentlyOverlapping = IsEllipseEllipseOverlapping(
            centerX,
            centerY,
            otherCenterX,
            otherCenterY,
            sumA,
            sumB
        );

        if (isCurrentlyOverlapping) {
            float newCenterX = newX + a;
            float newCenterY = newY + b;
            bool isOverlappingAtNewPosition = IsEllipseEllipseOverlapping(
                newCenterX,
                newCenterY,
                otherCenterX,
                otherCenterY,
                sumA,
                sumB
            );

            if (!isOverlappingAtNewPosition) {
                return std::nullopt;
            }

            float moveX = newX - currentX;
            float moveY = newY - currentY;
            auto [correctedCenterX, correctedCenterY] = ResolveEllipseOverlap(
                newCenterX,
                newCenterY,
                otherCenterX,
                otherCenterY,
                sumA,
                sumB,
                moveX,
                moveY
            );

            return std::make_tuple(correctedCenterX - a, correctedCenterY - b);
        }

        float moveX = newX - currentX;
        float moveY = newY - currentY;

        float px = (centerX - otherCenterX) / sumA;
        float py = (centerY - otherCenterY) / sumB;
        float vx = moveX / sumA;
        float vy = moveY / sumB;

        float A = vx * vx + vy * vy;
        if (A <= floatEpsilon) {
            return std::nullopt;
        }

        float B = 2.0f * (px * vx + py * vy);
        float C = px * px + py * py - 1.0f;

        float discriminant = B * B - 4.0f * A * C;
        if (discriminant < -floatEpsilon) {
            return std::nullopt;
        }
        discriminant = (std::max)(0.0f, discriminant);

        float sqrtDiscriminant = std::sqrt(discriminant);
        float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
        float t2 = (-B + sqrtDiscriminant) / (2.0f * A);
        if (t1 > t2) {
            std::swap(t1, t2);
        }

        float t = -1.0f;
        if (t1 >= -floatEpsilon && t1 <= (1.0f + floatEpsilon)) {
            t = t1;
        }
        else if (t2 >= -floatEpsilon && t2 <= (1.0f + floatEpsilon)) {
            t = t2;
        }

        if (t < -floatEpsilon) {
            return std::nullopt;
        }

        t = (std::max)(0.0f, (std::min)(1.0f, t));

        float correctedT = (std::max)(0.0f, t - collisionEpsilon);
        return std::make_tuple(currentX + moveX * correctedT, currentY + moveY * correctedT);
    }
    else if (auto rectangleCollider = dynamic_pointer_cast<RectangleCollider>(lock)) {
        return ICollider::IsIntersecting(dynamic_pointer_cast<EllipseCollider>(shared_from_this()), rectangleCollider, newX, newY);
    }
    else {
        throw std::runtime_error("Unknown collider, method has not implemented logic");
    }
}
