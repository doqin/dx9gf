#pragma once
#include <queue>
#include <memory>
#include "DX9GFICommand.h"
#include "DX9GFIGameObject.h"

namespace DX9GF {
	class CommandBuffer {
	private:
		std::mutex mutex;
		std::queue<std::shared_ptr<ICommand>> queue;
		std::shared_ptr<ICommand> currentCommand;
	protected:
	public:
		void PushCommand(std::shared_ptr<ICommand> command);
		void Update(unsigned long long deltaTime);
		bool IsBusy();
	};
}