#include "DX9GFCommandBuffer.h"

void DX9GF::CommandBuffer::PushCommand(std::shared_ptr<ICommand> command)
{
	{
		std::scoped_lock<std::mutex> lock{mutex};
		this->queue.push(command);
	}
}

void DX9GF::CommandBuffer::Update(unsigned long long deltaTime)
{
	if (currentCommand && currentCommand->IsFinished()) {
		currentCommand.reset();
	}

	if (!currentCommand) {
		if (queue.empty()) {
			return;
		}

		currentCommand = queue.front();
		queue.pop();
	}

	if (!currentCommand || currentCommand->IsFinished()) {
		currentCommand.reset();
		return;
	}

	currentCommand->Execute(deltaTime);

	if (currentCommand->IsFinished()) {
		currentCommand.reset();
	}
}

bool DX9GF::CommandBuffer::IsBusy()
{
	std::scoped_lock<std::mutex> lock{ mutex };
	return currentCommand != nullptr || !queue.empty();
}