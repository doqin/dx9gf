#include "pch.h"
#include "DX9GFConcurrentCommand.h"

DX9GF::ConcurrentCommand::ConcurrentCommand()
{
}

void DX9GF::ConcurrentCommand::AddCommand(std::shared_ptr<ICommand> command)
{
	commands.push_back(command);
}

void DX9GF::ConcurrentCommand::Execute(unsigned long long deltaTime)
{
	bool allFinished = true;

	for (auto& cmd : commands) {
		if (!cmd->IsFinished()) {
			cmd->Execute(deltaTime);

			if (!cmd->IsFinished()) {
				allFinished = false;
			}
		}
	}

	if (allFinished) {
		MarkFinished();
	}
}