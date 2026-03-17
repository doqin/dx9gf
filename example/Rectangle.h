#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"

namespace GO {
	class Rectangle : public DX9GF::IGameObject {
	private:
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		DX9GF::Camera* camera = nullptr;
		std::shared_ptr<DX9GF::RectangleTrigger> trigger = nullptr;
		std::shared_ptr<DX9GF::RectangleCollider> collider = nullptr;
		float width, height;
	protected:
	public:
		Rectangle(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			float width,
			float height,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY),
			width(width),
			height(height) {}
		Rectangle(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<IGameObject> parent,
			float width,
			float height,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY),
			width(width),
			height(height) {}
		std::weak_ptr<DX9GF::RectangleCollider> GetCollider();
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::shared_ptr<DX9GF::ColliderManager> colliderManager);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		void Dispose();
	};
}