#pragma once
#include "DX9GFICommand.h"
#include <vector>
#include <memory>

namespace DX9GF {
	class ConcurrentCommand : public ICommand {
	private:
		std::vector<std::shared_ptr<ICommand>> commands;
	public:
		ConcurrentCommand();
		void AddCommand(std::shared_ptr<ICommand> command);
		void Execute(unsigned long long deltaTime) override;
	};
}