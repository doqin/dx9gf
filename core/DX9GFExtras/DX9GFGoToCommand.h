#pragma once
#include "DX9GFICommand.h"
#include "DX9GFIGameObject.h"
#include <memory>

namespace DX9GF {
	class GoToCommand : public ICommand {
	private:
		std::weak_ptr<IGameObject> targetObject;
		float targetX;
		float targetY;
		float velocity; // Pixel per second
	public:
		GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float velocity);
		void Execute(unsigned long long deltaTime) override;
	};
}