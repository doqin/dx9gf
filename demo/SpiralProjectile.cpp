#include "pch.h"
#include "SpiralProjectile.h"
#include <cmath>

Demo::SpiralProjectile::Builder& Demo::SpiralProjectile::Builder::SetSpiralParams(float initialAngle, float radialSpd, float angularVel) {
	this->instance->currentAngle = initialAngle;
	this->instance->radialSpeed = radialSpd;
	this->instance->angularVelocity = angularVel;
	return *this;
}

Demo::SpiralProjectile::Builder& Demo::SpiralProjectile::Builder::SetDelay(float delay) {
	this->instance->delay = delay;
	return *this;
}

Demo::SpiralProjectile::Builder& Demo::SpiralProjectile::Builder::SetDecayTime(float decayTime) {
	this->instance->decayTime = decayTime;
	return *this;
}

Demo::SpiralProjectile::Builder& Demo::SpiralProjectile::Builder::SetDamage(float dmg) {
	this->instance->damage = dmg;
	return *this;
}

std::shared_ptr<Demo::SpiralProjectile> Demo::SpiralProjectile::Builder::Build() {
	return instance;
}

void Demo::SpiralProjectile::Init() {
	this->collider = std::make_shared<DX9GF::EllipseCollider>(transformManager, shared_from_this(), colliderWidth, colliderHeight);
	this->collider->SetOriginCenter();

	auto [startX, startY] = GetWorldPosition();
	this->originX = startX;
	this->originY = startY;
}

void Demo::SpiralProjectile::Update(unsigned long long deltaTime) {
	if (decayTime != UNSPECIFIED && this->elapsed >= decayTime) {
		state = State::Destroyed;
		return;
	}
	if (this->elapsed >= delay) {
		float dt = deltaTime / 1000.f;

		currentRadius += radialSpeed * dt;
		currentAngle += angularVelocity * dt;

		float newX = originX + currentRadius * std::cos(currentAngle);
		float newY = originY + currentRadius * std::sin(currentAngle);

		SetLocalPosition(newX, newY);
	}

	if (collider->IsCollided(player->GetCollider())) {
		player->TakeDamage(damage);
		state = State::Destroyed;
	}
	this->elapsed += deltaTime / 1000.f;
}

void Demo::SpiralProjectile::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) {
	if (this->elapsed < delay) return;
	this->sprite->Begin();
	auto [x, y] = GetWorldPosition();
	this->sprite->SetPosition(x, y);

	this->sprite->SetRotation(currentAngle);

	this->sprite->SetScaleX(GetWorldScaleX());
	this->sprite->SetScaleY(GetWorldScaleY());
	this->sprite->Draw(camera, deltaTime);
	this->sprite->End();
	collider->Draw(sprite->GetGraphicsDevice(), camera);
}