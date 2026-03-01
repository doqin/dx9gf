#include "DX9GFIGameObject.h"

void DX9GF::IGameObject::UpdateAbsoluteX()
{
    if (parent.has_value()) {
        auto ptr = parent.value();
        absoluteX = ptr->GetAbsoluteX() + relativeX;
    }
    else {
        absoluteX = relativeX;
    }
}

void DX9GF::IGameObject::UpdateAbsoluteY()
{
    if (parent.has_value()) {
        auto ptr = parent.value();
        absoluteY = ptr->GetAbsoluteY() + relativeY;
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
        auto ptr = parent.value();
        relativeX = ptr->GetAbsoluteX() - absoluteX;
    }
    else {
        relativeX = absoluteX;
    }
}

void DX9GF::IGameObject::UpdateRelativeY()
{
    if (parent.has_value()) {
        auto ptr = parent.value();
        relativeY = ptr->GetAbsoluteY() - absoluteY;
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

void DX9GF::IGameObject::UpdateChildren(unsigned long long deltaTime)
{
    for (int i = 0; i < children.size(); i++) {
        children[i].Update(deltaTime);
    }
}

DX9GF::IGameObject::IGameObject(IGameObject* parent, float x, float y, bool useAbsoluteCoords)
{
    auto [parentX, parentY] = parent->GetAbsolutePosition();
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

std::vector<DX9GF::IGameObject*> DX9GF::IGameObject::Flatten()
{
    std::vector<IGameObject*> array;
    array.push_back(this);
    for (int i = 0; i < children.size(); i++) {
        array.push_back(&children[i]);
    }
    return array;
}

const std::mutex& DX9GF::IGameObject::GetMutex()
{
    return this->objectMutex;
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
    UpdateChildren(deltaTime);
}

void DX9GF::IGameObject::Draw(unsigned long long deltaTime)
{
    for (int i = 0; i < children.size(); i++) {
        children[i].Draw(deltaTime);
    }
}

void DX9GF::IGameObject::Dispose()
{
    for (int i = 0; i < children.size(); i++) {
        children[i].Dispose();
    }
}
