#include "pch.h"
#include "DX9GFDestroyObjectCommand.h"

void DX9GF::DestroyObjectCommand::Execute(unsigned long long deltaTime)
{
	if (cleanup) {
		cleanup();
	}
	this->SetObjectState(gameObject, IGameObject::State::Destroyed);
	MarkFinished();
}
