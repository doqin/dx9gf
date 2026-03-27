#include "pch.h"
#include "DX9GFSetPositionCommand.h"

void DX9GF::SetPositionCommand::Execute(unsigned long long deltaTime)
{
	if (auto lock = obj.lock()) {
		lock->SetLocalPosition(x, y);
	}
	MarkFinished();
}
