#pragma once
#include "DX9GFIGameObject.h"
#include <functional>
#include "../DX9GFCamera.h"

namespace DX9GF {
	class ITrigger : public IGameObject {
	private:
	protected:
		Camera* camera;
		bool isHeldLeft = false;
		bool isHeldRight = false;
		std::function<void(ITrigger*)> onHover = [](ITrigger*) {};
		std::function<void(ITrigger*)> onClickLeft = [](ITrigger*) {};
		std::function<void(ITrigger*)> onClickRight = [](ITrigger*) {};
		std::function<void(ITrigger*)> onHeldLeft = [](ITrigger*) {};
		std::function<void(ITrigger*)> onHeldRight = [](ITrigger*) {};
		ITrigger(std::weak_ptr<TransformManager> transformManager) : IGameObject(transformManager) {}
		ITrigger(std::weak_ptr<TransformManager> transformManager, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) {}
		ITrigger(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<IGameObject> parent, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY) {}
	public:
		void Init(Camera* camera);
		virtual bool IsHovering(unsigned long long deltaTime) = 0;
		bool IsClicked(unsigned long long deltaTime);
		bool IsClickedLeft(unsigned long long deltaTime);
		bool IsClickedRight(unsigned long long deltaTime);
		bool IsHeld(unsigned long long deltaTime);
		bool IsHeldLeft(unsigned long long deltaTime);
		bool IsHeldRight(unsigned long long deltaTime);
		void SetOnHover(std::function<void(ITrigger*)> onHover);
		void SetOnClickLeft(std::function<void(ITrigger*)> onClick);
		void SetOnClickRight(std::function<void(ITrigger*)> onClick);
		void SetOnHeldLeft(std::function<void(ITrigger*)> onHeld);
		void SetOnHeldRight(std::function<void(ITrigger*)> onHeld);
		void Update(unsigned long long deltaTime);
	public:
		static bool debugDraw;
	};
}