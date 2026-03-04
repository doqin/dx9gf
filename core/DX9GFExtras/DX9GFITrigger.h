#pragma once
#include "DX9GFIGameObject.h"
#include <functional>

namespace DX9GF {
	class ITrigger : public IGameObject {
	private:
	protected:
		bool isHeld = false;
		std::function<void(ITrigger*)> onHover = [](ITrigger*) {};
		std::function<void(ITrigger*)> onClick = [](ITrigger*) {};
		std::function<void(ITrigger*)> onHeld = [](ITrigger*) {};
		ITrigger(std::weak_ptr<TransformManager> transformManager) : IGameObject(transformManager) {}
		ITrigger(std::weak_ptr<TransformManager> transformManager, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) {}
		ITrigger(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<IGameObject> parent, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY) {}
	public:
		virtual bool IsHovering(unsigned long long deltaTime) = 0;
		bool IsClicked(unsigned long long deltaTime);
		bool IsHeld(unsigned long long deltaTime);
		void SetOnHover(std::function<void(ITrigger*)> onHover);
		void SetOnClick(std::function<void(ITrigger*)> onClick);
		void SetOnHeld(std::function<void(ITrigger*)> onHeld);
		void Update(unsigned long long deltaTime);
	};
}