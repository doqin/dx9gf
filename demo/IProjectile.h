#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"

namespace Demo {
	constexpr float UNSPECIFIED = -1;

	class IProjectile : public DX9GF::IGameObject {
	protected:
		std::shared_ptr<Player> player;
	public:
		IProjectile(std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player) : IGameObject(tm), player(player) {}
		IProjectile(std::weak_ptr<DX9GF::TransformManager> tm, std::shared_ptr<Player> player, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(tm, x, y, rotation, scaleX, scaleY), player(player) {}
		IProjectile(std::weak_ptr<DX9GF::TransformManager> tm, std::weak_ptr<DX9GF::IGameObject> parent, std::shared_ptr<Player> player, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(tm, parent, x, y, rotation, scaleX, scaleY), player(player) {}
		virtual void Init() = 0;
		virtual void Update(unsigned long long deltaTime) = 0;
		virtual void Draw(const DX9GF::Camera& camera, unsigned long long deltaTime) = 0;
	};
}