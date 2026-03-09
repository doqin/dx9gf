#include "DX9GFIGameObject.h"

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager) : transformManager(transformManager)
{
    this->transformHandle = transformManager.lock()->CreateTransform();
}

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager, float x, float y, float rotation, float scaleX, float scaleY) : transformManager(transformManager)
{
    this->transformHandle = transformManager.lock()->CreateTransform(NO_PARENT, x, y, rotation, scaleX, scaleY);
}

DX9GF::IGameObject::IGameObject(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<IGameObject> parent, float x, float y, float rotation, float scaleX, float scaleY) : transformManager(transformManager)
{
    this->transformHandle = transformManager.lock()->CreateTransform(parent.lock()->GetTransformHandle().slotIndex, x, y, rotation, scaleX, scaleY);
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
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localX = x;
}

void DX9GF::IGameObject::SetLocalY(float y)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localY = y;
}

void DX9GF::IGameObject::SetLocalPosition(float x, float y)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localX = x;
    transformData.localY = y;
}

void DX9GF::IGameObject::SetLocalRotation(float r)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localRotation = r;
}

void DX9GF::IGameObject::SetLocalScale(float sx, float sy)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localScaleX = sx;
    transformData.localScaleY = sy;
}

void DX9GF::IGameObject::SetLocalScaleX(float sx)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localScaleX = sx;
}

void DX9GF::IGameObject::SetLocalScaleY(float sy)
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    transformData.localScaleY = sy;
}

float DX9GF::IGameObject::GetLocalX() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.localX;
}

float DX9GF::IGameObject::GetLocalY() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.localY;
}

std::tuple<float, float> DX9GF::IGameObject::GetLocalPosition()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return std::tuple<float, float>(transformData.localX, transformData.localY);
}

float DX9GF::IGameObject::GetWorldX() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.worldX;
}

float DX9GF::IGameObject::GetWorldY() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.worldY;
}

std::tuple<float, float> DX9GF::IGameObject::GetWorldPosition()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return std::tuple<float, float>(transformData.worldX, transformData.worldY);
}

float DX9GF::IGameObject::GetLocalRotation() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.localRotation;
}

float DX9GF::IGameObject::GetWorldRotation() const
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.worldRotation;
}

std::tuple<float, float> DX9GF::IGameObject::GetLocalScale()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return std::tuple<float, float>(transformData.localScaleX, transformData.localScaleY);
}

float DX9GF::IGameObject::GetLocalScaleX()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.localScaleX;
}

float DX9GF::IGameObject::GetLocalScaleY()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.localScaleY;
}

std::tuple<float, float> DX9GF::IGameObject::GetWorldScale()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return std::tuple<float, float>(transformData.worldScaleX, transformData.worldScaleY);
}

float DX9GF::IGameObject::GetWorldScaleX()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.worldScaleX;
}

float DX9GF::IGameObject::GetWorldScaleY()
{
    auto lock = transformManager.lock();
    auto& transformData = lock->GetTransform(transformHandle.slotIndex);
    return transformData.worldScaleY;
}
