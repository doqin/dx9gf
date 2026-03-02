#include "DX9GFIGameObject.h"

void DX9GF::IGameObject::UpdateAbsoluteX()
{
    if (parent.has_value()) {
        if (auto lock = parent.value().lock()) {
            absoluteX = lock->GetAbsoluteX() + relativeX;
        }  
    }
    else {
        absoluteX = relativeX;
    }
}

void DX9GF::IGameObject::UpdateAbsoluteY()
{
    if (parent.has_value()) {
        if (auto lock = parent.value().lock()) {
            absoluteY = lock->GetAbsoluteY() + relativeY;
        }
    }
    else {
        absoluteY = relativeY;
    }
}

void DX9GF::IGameObject::UpdateAbsolutePosition()
{
    UpdateAbsoluteX();
    UpdateAbsoluteY();
}

void DX9GF::IGameObject::UpdateRelativeX()
{
    if (parent.has_value()) {
        if (auto lock = parent.value().lock()) {
            relativeX = lock->GetAbsoluteX() - absoluteX;
        }
        
    }
    else {
        relativeX = absoluteX;
    }
}

void DX9GF::IGameObject::UpdateRelativeY()
{
    if (parent.has_value()) {
        if (auto lock = parent.value().lock()) {
            relativeY = lock->GetAbsoluteY() - absoluteY;
        }
    }
    else {
        relativeY = absoluteY;
    }
}

void DX9GF::IGameObject::UpdateRelativePosition()
{
    UpdateRelativeX();
    UpdateRelativeY();
}

DX9GF::IGameObject::IGameObject(std::weak_ptr<IGameObject> parent, float x, float y, bool useAbsoluteCoords)
{
    this->parent = parent;
    if (auto lock = parent.lock()) {
        auto [parentX, parentY] = lock->GetAbsolutePosition();
        if (useAbsoluteCoords) {
            absoluteX = x;
            absoluteY = y;
            relativeX = parentX - absoluteX;
            relativeY = parentY - absoluteY;
        }
        else {
            relativeX = x;
            relativeY = y;
            absoluteX = parentX + relativeX;
            absoluteY = parentY + relativeY;
        }
    }
}

void DX9GF::IGameObject::MainUpdate(unsigned long long deltaTime)
{
}

DX9GF::IGameObject::~IGameObject()
{
    this->Dispose();
}

std::mutex& DX9GF::IGameObject::GetMutex()
{
    return this->objectMutex;
}

std::optional<std::weak_ptr<DX9GF::IGameObject>> DX9GF::IGameObject::GetParent() const
{
    return parent;
}

void DX9GF::IGameObject::SetRelativeX(float x)
{
    relativeX = x;
    UpdateAbsoluteX();
}

void DX9GF::IGameObject::SetRelativeY(float y)
{
    relativeY = y;
    UpdateAbsoluteY();
}

void DX9GF::IGameObject::SetRelativePosition(float x, float y)
{
    SetRelativeX(x);
    SetRelativeY(y);
}

void DX9GF::IGameObject::SetAbsoluteX(float x)
{
    absoluteX = x;
    UpdateRelativeX();
}

void DX9GF::IGameObject::SetAbsoluteY(float y)
{
    absoluteY = y;
    UpdateRelativeY();
}

void DX9GF::IGameObject::SetAbsolutePosition(float x, float y)
{
    SetAbsoluteX(x);
    SetAbsoluteY(y);
}

float DX9GF::IGameObject::GetRelativeX() const
{
    return relativeX;
}

float DX9GF::IGameObject::GetRelativeY() const
{
    return relativeY;
}

std::tuple<float, float> DX9GF::IGameObject::GetRelativePosition()
{
    return std::tuple<float, float>(relativeX, relativeY);
}

float DX9GF::IGameObject::GetAbsoluteX() const
{
    return absoluteX;
}

float DX9GF::IGameObject::GetAbsoluteY() const
{
    return absoluteY;
}

std::tuple<float, float> DX9GF::IGameObject::GetAbsolutePosition()
{
    return std::tuple<float, float>(absoluteX, absoluteY);
}

void DX9GF::IGameObject::Update(unsigned long long deltaTime)
{
    UpdateAbsolutePosition();
    MainUpdate(deltaTime);
}

void DX9GF::IGameObject::Draw(unsigned long long deltaTime)
{
}

void DX9GF::IGameObject::Dispose()
{
}
