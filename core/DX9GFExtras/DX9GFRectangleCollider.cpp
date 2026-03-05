#include "DX9GFRectangleCollider.h"
#include "../DX9GFUtils.h"
#include <algorithm>
#include <cmath>
#include <limits>

bool DX9GF::RectangleCollider::IsCollidedRectangle(std::weak_ptr<RectangleCollider> other, bool checkOther)
{
    auto lock = other.lock();
    auto otherX = lock->GetWorldX();
    auto otherY = lock->GetWorldY();
    auto otherWidth = lock->GetWidth();
    auto otherHeight = lock->GetHeight();
    auto isCollided =
        Utils::IsWithinRectangle(GetWorldX(), GetWorldY(), otherX, otherY, otherWidth, otherHeight) // top left corner
        && Utils::IsWithinRectangle(GetWorldX() + width, GetWorldY(), otherX, otherY, otherWidth, otherHeight) // top right corner
        && Utils::IsWithinRectangle(GetWorldX() + width, GetWorldY() + height, otherX, otherY, otherWidth, otherHeight) // bottom right corner
        && Utils::IsWithinRectangle(GetWorldX(), GetWorldY() + height, otherX, otherY, otherWidth, otherHeight); // bottom left corner
    if (checkOther) {
        return isCollided && other.lock()->IsCollidedRectangle(dynamic_pointer_cast<RectangleCollider>(shared_from_this()), false);
    }
    else {
        return isCollided;
    }
}

bool DX9GF::RectangleCollider::IsCollided(std::weak_ptr<ICollider> other)
{
    auto lock = other.lock();
    if (auto rectangleCollider = dynamic_pointer_cast<RectangleCollider>(lock)) {
        return IsCollidedRectangle(rectangleCollider, true);
    }
    else {
        throw std::runtime_error("Unknown collider, method has not implemented logic");
    }
}

bool DX9GF::RectangleCollider::CheckAxisIntersection(float current, float delta, float expandedMin, float expandedMax, float& tNear, float& tFar) const
{
    if (delta == 0) {
        if (current < expandedMin || current > expandedMax) {
            return false;
        }
    }
    else {
        float t1 = (expandedMin - current) / delta;
        float t2 = (expandedMax - current) / delta;
        if (t1 > t2) std::swap(t1, t2);
        tNear = (std::max)(tNear, t1);
        tFar = (std::min)(tFar, t2);
        if (tNear > tFar) return false;
    }
    return true;
}

// Claude did my homework :DDDD
std::optional<std::tuple<float, float>> DX9GF::RectangleCollider::IsIntersecting(std::weak_ptr<ICollider> other, float newX, float newY)
{
    auto lock = other.lock();
    if (auto rectangleCollider = dynamic_pointer_cast<RectangleCollider>(lock)) {
        float currentX = GetWorldX();
        float currentY = GetWorldY();

        float otherX = rectangleCollider->GetWorldX();
        float otherY = rectangleCollider->GetWorldY();
        float otherWidth = rectangleCollider->GetWidth();
        float otherHeight = rectangleCollider->GetHeight();

        float dx = newX - currentX;
        float dy = newY - currentY;

        bool isOverlappingNow =
            currentX < otherX + otherWidth
            && currentX + width > otherX
            && currentY < otherY + otherHeight
            && currentY + height > otherY;

        if (isOverlappingNow) {
            float pushLeft = (currentX + width) - otherX;
            float pushRight = (otherX + otherWidth) - currentX;
            float pushUp = (currentY + height) - otherY;
            float pushDown = (otherY + otherHeight) - currentY;

            float correctionX = -pushLeft;
            float correctionY = 0.0f;
            float shortest = pushLeft;

            if (pushRight < shortest) {
                shortest = pushRight;
                correctionX = pushRight;
                correctionY = 0.0f;
            }
            if (pushUp < shortest) {
                shortest = pushUp;
                correctionX = 0.0f;
                correctionY = -pushUp;
            }
            if (pushDown < shortest) {
                correctionX = 0.0f;
                correctionY = pushDown;
            }

            float epsilon = 0.001f;
            if (correctionX != 0.0f) {
                correctionX += (correctionX > 0 ? epsilon : -epsilon);
            }
            if (correctionY != 0.0f) {
                correctionY += (correctionY > 0 ? epsilon : -epsilon);
            }

            return std::make_tuple(currentX + correctionX, currentY + correctionY);
        }

        float expandedX = otherX - width;
        float expandedY = otherY - height;
        float expandedWidth = otherWidth + width;
        float expandedHeight = otherHeight + height;

        float tNear = -std::numeric_limits<float>::infinity();
        float tFar = std::numeric_limits<float>::infinity();

        if (!CheckAxisIntersection(currentX, dx, expandedX, expandedX + expandedWidth, tNear, tFar)) {
            return std::nullopt;
        }

        if (!CheckAxisIntersection(currentY, dy, expandedY, expandedY + expandedHeight, tNear, tFar)) {
            return std::nullopt;
        }

        if (tNear <= 1 && tFar >= 0) {
            float epsilon = 0.001f;
            float t = (std::max)(0.0f, tNear - epsilon);
            float correctedX = currentX + dx * t;
            float correctedY = currentY + dy * t;
            return std::make_tuple(correctedX, correctedY);
        }

        return std::nullopt;
    }
    else {
        throw std::runtime_error("Unknown collider, method has not implemented logic");
    }
}

float DX9GF::RectangleCollider::GetWidth() const
{
    return width;
}

float DX9GF::RectangleCollider::GetHeight() const
{
    return height;
}
