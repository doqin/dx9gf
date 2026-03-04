#include "DX9GFIGameObject.h"

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager) : transformManager(transformManager)
{
    if (auto lock = transformManager.lock()) {
        this->transformHandle = lock->CreateTransform();
    }
}

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager, float x, float y, float rotation, float scaleX, float scaleY) : transformManager(transformManager)
{
    if (auto lock = transformManager.lock()) {
        this->transformHandle = lock->CreateTransform(NO_PARENT, x, y, rotation, scaleX, scaleY);
    }
}

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<IGameObject> parent, float x, float y, float rotation, float scaleX, float scaleY) : transformManager(transformManager)
{
    if (auto lock = transformManager.lock()) {
        if (auto parentLock = parent.lock()) {
            this->transformHandle = lock->CreateTransform(parentLock->GetTransformHandle().slotIndex, x, y, rotation, scaleX, scaleY);
        }
    }
    this->parent = parent;
}

DX9GF::IGameObject::~IGameObject()
{
    
}

std::optional<std::weak_ptr<DX9GF::IGameObject>> DX9GF::IGameObject::GetParent() const
{
    return parent;
}

DX9GF::TransformHandle DX9GF::IGameObject::GetTransformHandle() const
{
    return transformHandle;
}

void DX9GF::IGameObject::SetLocalX(float x)
{
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        transformData.localX = x;
    }
}

void DX9GF::IGameObject::SetLocalY(float y)
{
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        transformData.localY = y;
    }
}

void DX9GF::IGameObject::SetLocalPosition(float x, float y)
{
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        transformData.localX = x;
        transformData.localY = y;
    }
    else {
        throw std::runtime_error("Could not get lock");
    }
}

float DX9GF::IGameObject::GetLocalX() const
{
    float x = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        x = transformData.localX;
    }
    return x;
}

float DX9GF::IGameObject::GetLocalY() const
{
    float y = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        y = transformData.localY;
    }
    return y;
}

std::tuple<float, float> DX9GF::IGameObject::GetLocalPosition()
{
    float x = 0;
    float y = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        x = transformData.localX;
        y = transformData.localY;
    }
    return std::tuple<float, float>(x, y);
}

float DX9GF::IGameObject::GetWorldX() const
{
    float x = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        x = transformData.worldX;
    }
    return x;
}

float DX9GF::IGameObject::GetWorldY() const
{
    float y = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        y = transformData.worldY;
    }
    return y;
}

std::tuple<float, float> DX9GF::IGameObject::GetWorldPosition()
{
    float x = 0;
    float y = 0;
    if (auto lock = transformManager.lock()) {
        auto& transformData = lock->GetTransform(transformHandle.slotIndex);
        x = transformData.worldX;
        y = transformData.worldY;
    }
    return std::tuple<float, float>(x, y);
}
