#pragma once
#include "DX9GFICommand.h"
#include "DX9GFIGameObject.h"
#include <memory>

namespace DX9GF {
	struct VelocityTag{};
	struct TimeTag{};
   struct EaseInOutTag{};
	class GoToCommand : public ICommand {
	private:
		std::weak_ptr<IGameObject> targetObject;
		float targetX;
		float targetY;
		float velocity; // Pixel per second
		bool useEaseInOut;
		float duration;
		float elapsedTime;
		float startX;
		float startY;
		bool initialized;
	public:
		GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float velocity);
		GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float velocity, VelocityTag);
		GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float duration, TimeTag);
        GoToCommand(std::weak_ptr<IGameObject> gameObject, float x, float y, float duration, TimeTag, EaseInOutTag);
		void Execute(unsigned long long deltaTime) override;
	};
}