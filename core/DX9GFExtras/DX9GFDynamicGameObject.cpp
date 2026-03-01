#include "DX9GFDynamicGameObject.h"

std::unordered_map<std::string, void*>& DX9GF::DynamicGameObject::GetFieldDictionary()
{
    return fieldDictionary;
}

void DX9GF::DynamicGameObject::SetUpdateFunction(std::function<void(DynamicGameObject*, unsigned long long)> updateFunction)
{
    this->updateFunction = updateFunction;
}

void DX9GF::DynamicGameObject::SetDrawFunction(std::function<void(DynamicGameObject*, unsigned long long)> drawFunction)
{
    this->drawFunction = drawFunction;
}

void DX9GF::DynamicGameObject::SetDisposeFunction(std::function<void(DynamicGameObject*)> disposeFunction)
{
    this->disposeFunction = disposeFunction;
}

void DX9GF::DynamicGameObject::Init(std::function<void(DynamicGameObject*)> initFunction)
{
    initFunction(this);
}

void DX9GF::DynamicGameObject::MainUpdate(unsigned long long deltaTime)
{
    this->updateFunction(this, deltaTime);
}

void DX9GF::DynamicGameObject::Draw(unsigned long long deltaTime)
{
    this->drawFunction(this, deltaTime);
}

void DX9GF::DynamicGameObject::Dispose()
{
    this->disposeFunction(this);
}
