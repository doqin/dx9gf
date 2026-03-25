#pragma once
#include "DX9GFICommand.h"

namespace DX9GF {
	class MultiCommand : public ICommand {
		std::queue<std::shared_ptr<ICommand>> queue;
		std::shared_ptr<ICommand> currentCommand;
	public:
		MultiCommand(std::vector<std::shared_ptr<ICommand>>&& commandList);
		void Execute(unsigned long long deltaTime) override;
	};
}