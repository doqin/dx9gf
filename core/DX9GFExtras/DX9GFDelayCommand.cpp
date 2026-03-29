#include "pch.h"
#include "DX9GFDelayCommand.h"

void DX9GF::DelayCommand::Execute(unsigned long long deltaTime)
{
	if (elapsed > delayTime) {
		MarkFinished();
	}
	elapsed += deltaTime / 1000.f;
}
