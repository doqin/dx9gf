#pragma once
#include "DX9GFICommand.h"
namespace DX9GF {
	class CustomCommand : public ICommand {
		std::function<void(std::function<void(void)> markFinished)> customCommand;
	public:
		CustomCommand(std::function<void(std::function<void(void)> markFinished)> customCommand) : customCommand(customCommand) {}
		void Execute(unsigned long long deltaTime) override;
	};
}