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
		ITrigger() : IGameObject() {}
		ITrigger(float x, float y) : IGameObject(x, y) {}
		ITrigger(std::weak_ptr<IGameObject> parent, float x, float y, bool useAbsoluteCoords = false) : IGameObject(parent, x, y, useAbsoluteCoords) {}
		virtual void MainUpdate(unsigned long long deltaTime) override;
	public:
		virtual bool IsHovering(unsigned long long deltaTime) = 0;
		bool IsClicked(unsigned long long deltaTime);
		bool IsHeld(unsigned long long deltaTime);
		virtual void Draw(unsigned long long deltaTime) override;
		virtual void Dispose() override;
		void SetOnHover(std::function<void(ITrigger*)> onHover);
		void SetOnClick(std::function<void(ITrigger*)> onClick);
		void SetOnHeld(std::function<void(ITrigger*)> onHeld);
	};
}