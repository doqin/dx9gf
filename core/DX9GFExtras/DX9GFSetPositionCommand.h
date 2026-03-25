#pragma once
#include "DX9GFICommand.h"
#include "DX9GFIGameObject.h"

namespace DX9GF {
	class SetPositionCommand : public ICommand {
		std::weak_ptr<IGameObject> obj;
		float x, y;
	public:
		SetPositionCommand(std::weak_ptr<IGameObject> obj, float x, float y) : obj(obj), x(x), y(y) {}
		void Execute(unsigned long long deltaTime) override;
	};
}