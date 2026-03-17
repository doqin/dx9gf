#pragma once
#include "DX9GFExtras.h"
#include "DX9GF.h"

namespace GO {
	class Player : public DX9GF::IGameObject, DX9GF::ISaveable {
		inline static size_t playerCount = 0;
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		DX9GF::Camera* camera = nullptr;
		DX9GF::CommandBuffer* commandBuffer = nullptr;
		std::shared_ptr<DX9GF::Texture> marioTexture = nullptr;
		std::shared_ptr<DX9GF::AnimatedSprite> mario = nullptr;
		std::shared_ptr<DX9GF::RectangleCollider> collider = nullptr;
		std::shared_ptr<DX9GF::ColliderManager> colliderManager = nullptr;
	public:
		Player(std::weak_ptr<DX9GF::TransformManager> transformManager) : IGameObject(transformManager) { playerCount++; }
		Player(std::weak_ptr<DX9GF::TransformManager> transformManager, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) { playerCount++; }
		~Player();
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::CommandBuffer* commandBuffer, DX9GF::Camera* camera, std::shared_ptr<DX9GF::ColliderManager> colliderManager);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);

		// Inherited via ISaveable
		std::string GetSaveID() const override;
		void GenerateSaveData(nlohmann::json& outData) override;
		void RestoreSaveData(const nlohmann::json& inData) override;
	};
}