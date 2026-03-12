#include "DX9GFICommand.h"

void DX9GF::ICommand::MarkFinished()
{
    isFinished = true;
}

void DX9GF::ICommand::SetObjectState(std::weak_ptr<IGameObject> object, IGameObject::State state)
{
    if (auto lock = object.lock()) {
        lock->state = state;
    }
}

bool DX9GF::ICommand::IsFinished() const
{
    return isFinished;
}
