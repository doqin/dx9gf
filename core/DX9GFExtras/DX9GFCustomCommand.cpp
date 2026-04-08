#include "pch.h"
#include "DX9GFCustomCommand.h"
#include <functional>

void DX9GF::CustomCommand::Execute(unsigned long long deltaTime)
{
	customCommand([this]() { MarkFinished(); });
}
