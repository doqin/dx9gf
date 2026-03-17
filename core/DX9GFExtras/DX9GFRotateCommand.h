#pragma once
#include "DX9GFICommand.h"
#include "DX9GFIGameObject.h"
#include <memory>

namespace DX9GF {
	class RotateCommand : public ICommand {
	private:
		std::weak_ptr<IGameObject> targetObject;
		float targetRotation;
		float angularVelocity;
	public:
		RotateCommand(std::weak_ptr<IGameObject> gameObject, float targetRotation, float angularVelocity);
		void Execute(unsigned long long deltaTime) override;
	};
}