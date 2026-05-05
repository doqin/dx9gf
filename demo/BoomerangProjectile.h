#pragma once
#include "IProjectile.h"

namespace Demo {
	class BoomerangProjectile : public IProjectile {
	public:
		class Builder;
		friend class Builder;
	private:
		std::shared_ptr<DX9GF::EllipseCollider> collider;
		std::shared_ptr<DX9GF::ISprite> sprite;
		float colliderWidth;
		float colliderHeight;
		float velocity;
		float returnAcceleration;
		D3DXVECTOR2 trajectory;
		float delay = 0.f;
		float elapsed = 0.f;
		float decayTime = UNSPECIFIED;
		float damage = 0.f;

	protected:
		BoomerangProjectile(
			std::weak_ptr<DX9GF::TransformManager> tm,
			std::shared_ptr<Player> player,
			std::shared_ptr<DX9GF::ISprite> sprite,
			float colliderWidth,
			float colliderHeight
		) : IProjectile(tm, player), sprite(sprite), colliderWidth(colliderWidth), colliderHeight(colliderHeight) {
		}

		BoomerangProjectile(
			std::weak_ptr<DX9GF::TransformManager> tm,
			std::shared_ptr<Player> player,
			std::shared_ptr<DX9GF::ISprite> sprite,
			float colliderWidth,
			float colliderHeight,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IProjectile(tm, player, x, y, rotation, scaleX, scaleY), sprite(sprite), colliderWidth(colliderWidth), colliderHeight(colliderHeight) {
		}

		BoomerangProjectile(
			std::weak_ptr<DX9GF::TransformManager> tm,
			std::weak_ptr<DX9GF::IGameObject> parent,
			std::shared_ptr<Player> player,
			std::shared_ptr<DX9GF::ISprite> sprite,
			float colliderWidth,
			float colliderHeight,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IProjectile(tm, parent, player, x, y, rotation, scaleX, scaleY), sprite(sprite), colliderWidth(colliderWidth), colliderHeight(colliderHeight) {
		}

	public:
		void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) override;
	};

	class BoomerangProjectile::Builder {
		std::shared_ptr<BoomerangProjectile> instance;
	public:
		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight
		) : instance(std::shared_ptr<BoomerangProjectile>(new BoomerangProjectile(tm, player, sprite, colliderWidth, colliderHeight))) {
		}

		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1
		) : instance(std::shared_ptr<BoomerangProjectile>(new BoomerangProjectile(tm, player, sprite, colliderWidth, colliderHeight, x, y, rotation, scaleX, scaleY))) {
		}

		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::weak_ptr<DX9GF::IGameObject> parent, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1
		) : instance(std::shared_ptr<BoomerangProjectile>(new BoomerangProjectile(tm, parent, player, sprite, colliderWidth, colliderHeight, x, y, rotation, scaleX, scaleY))) {
		}

		Builder& SetTrajectory(D3DXVECTOR2 trajectory);
		Builder& SetTargetPosition(float x, float y);
		Builder& SetInitialVelocity(float velocity);
		Builder& SetReturnAcceleration(float acceleration);
		Builder& SetDelay(float delay);
		Builder& SetDecayTime(float decayTime);
		Builder& SetDamage(float dmg);
		std::shared_ptr<BoomerangProjectile> Build();
	};
}