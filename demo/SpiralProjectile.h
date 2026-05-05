#pragma once
#include "IProjectile.h"

namespace Demo {
	class SpiralProjectile : public IProjectile {
	public:
		class Builder;
		friend class Builder;
	private:
		std::shared_ptr<DX9GF::EllipseCollider> collider;
		std::shared_ptr<DX9GF::ISprite> sprite;
		float colliderWidth;
		float colliderHeight;

		float originX = 0.f;
		float originY = 0.f;
		float currentRadius = 0.f;
		float currentAngle = 0.f;

		float radialSpeed;   
		float angularVelocity;

		float delay = 0.f;
		float elapsed = 0.f;
		float decayTime = UNSPECIFIED;
		float damage = 0.f;

	protected:
		SpiralProjectile(
			std::weak_ptr<DX9GF::TransformManager> tm,
			std::shared_ptr<Player> player,
			std::shared_ptr<DX9GF::ISprite> sprite,
			float colliderWidth,
			float colliderHeight
		) : IProjectile(tm, player), sprite(sprite), colliderWidth(colliderWidth), colliderHeight(colliderHeight) {
		}

		SpiralProjectile(
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

		SpiralProjectile(
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

	class SpiralProjectile::Builder {
		std::shared_ptr<SpiralProjectile> instance;
	public:
		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight
		) : instance(std::shared_ptr<SpiralProjectile>(new SpiralProjectile(tm, player, sprite, colliderWidth, colliderHeight))) {
		}

		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1
		) : instance(std::shared_ptr<SpiralProjectile>(new SpiralProjectile(tm, player, sprite, colliderWidth, colliderHeight, x, y, rotation, scaleX, scaleY))) {
		}

		Builder(
			std::weak_ptr<DX9GF::TransformManager> tm, std::weak_ptr<DX9GF::IGameObject> parent, std::shared_ptr<Player> player, std::shared_ptr<DX9GF::ISprite> sprite, float colliderWidth, float colliderHeight, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1
		) : instance(std::shared_ptr<SpiralProjectile>(new SpiralProjectile(tm, parent, player, sprite, colliderWidth, colliderHeight, x, y, rotation, scaleX, scaleY))) {
		}

		Builder& SetSpiralParams(float initialAngle, float radialSpd, float angularVel);
		Builder& SetDelay(float delay);
		Builder& SetDecayTime(float decayTime);
		Builder& SetDamage(float dmg);
		std::shared_ptr<SpiralProjectile> Build();
	};
}