#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
namespace GO {
	class Ellipse : public DX9GF::IGameObject {
	private:
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		DX9GF::Camera* camera = nullptr;
		std::shared_ptr<DX9GF::EllipseTrigger> trigger = nullptr;
		std::shared_ptr<DX9GF::EllipseCollider> collider = nullptr;
		float width, height;
	protected:
	public:
		Ellipse(
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
			height(height) { }
		Ellipse(
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
			height(height) { }
		std::weak_ptr<DX9GF::EllipseCollider> GetCollider();
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::shared_ptr<DX9GF::ICollider>>* worldColliders);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		void Dispose();
	};
}