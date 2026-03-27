#include "pch.h"
#include "DX9GFCustomCommand.h"

void DX9GF::CustomCommand::Execute(unsigned long long deltaTime)
{
	customCommand([this]() { MarkFinished(); });
}
