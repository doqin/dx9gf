#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"

namespace Demo {
	class Player : public DX9GF::IGameObject {
	private:
		enum class State {
			Left,
			Right,
			Up,
			Down
		};
		// States
		State state = State::Down;
		bool isWalking = false;
		// Sprites
		std::shared_ptr<DX9GF::Texture> spritesheet;
		std::shared_ptr<DX9GF::StaticSprite> idleDown;
		std::shared_ptr<DX9GF::StaticSprite> idleUp;
		std::shared_ptr<DX9GF::StaticSprite> idleRight;
		std::shared_ptr<DX9GF::StaticSprite> idleLeft;
		std::shared_ptr<DX9GF::AnimatedSprite> walkingDown;
		std::shared_ptr<DX9GF::AnimatedSprite> walkingUp;
		std::shared_ptr<DX9GF::AnimatedSprite> walkingRight;
		std::shared_ptr<DX9GF::AnimatedSprite> walkingLeft;
		// Colliders
		std::shared_ptr<DX9GF::RectangleCollider> collider;
		// Externals
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		DX9GF::ColliderManager* colliderManager = nullptr;
		DX9GF::Camera* camera = nullptr;
		// Constants
		const float VELOCITY = 75;
		const float SPRINT_MULTIPLIER = 1.5f;
	public:
		Player(std::weak_ptr<DX9GF::TransformManager> transformManager) : IGameObject(transformManager) { }
		Player(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) { }
		Player(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<DX9GF::IGameObject> parent,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY) { }
		~Player();
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::ColliderManager* colliderManager, DX9GF::Camera* camera);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
	};
}