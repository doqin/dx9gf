#pragma once
#include "DX9GFExtras.h"
#include "DX9GF.h"

namespace GO {
	class Player : public DX9GF::IGameObject {
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		DX9GF::Camera* camera = nullptr;
		std::shared_ptr<DX9GF::Texture> marioTexture = nullptr;
		std::shared_ptr<DX9GF::AnimatedSprite> mario = nullptr;
		std::shared_ptr<DX9GF::RectangleCollider> collider = nullptr;
		std::vector<std::weak_ptr<DX9GF::ICollider>>* worldColliders;
	public:
		Player(std::weak_ptr<DX9GF::TransformManager> transformManager) : IGameObject(transformManager) {}
		Player(std::weak_ptr<DX9GF::TransformManager> transformManager, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) {}
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::weak_ptr<DX9GF::ICollider>>* worldColliders);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
	};
}