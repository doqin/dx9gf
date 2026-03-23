#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"
#include "IProjectile.h"
#include <functional>
#include <vector>

namespace Demo {
	class IEnemy : public DX9GF::IGameObject {
	private:
		const float maxHealth;
	protected:
       struct DamageIndicator {
			std::wstring text;
			float offsetY = 0.f;
			unsigned long long elapsed = 0;
		};
		float health;
        std::shared_ptr<DX9GF::RectangleTrigger> cardSpawnTrigger;
		std::function<void(std::shared_ptr<IEnemy>)> onRequestEnemyCard = [](std::shared_ptr<IEnemy>) {};
        std::vector<DamageIndicator> damageIndicators;
		std::shared_ptr<DX9GF::Font> damageFont;
		std::shared_ptr<DX9GF::FontSprite> damageFontSprite;
		std::vector<std::shared_ptr<IProjectile>> projectiles;
		DX9GF::CommandBuffer commandBuffer;
	public:
		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, float maxHealth) : IGameObject(tm), maxHealth(maxHealth), health(maxHealth) {}
		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, float maxHealth, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(tm, x, y, rotation, scaleX, scaleY), maxHealth(maxHealth), health(maxHealth) {}
		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, std::weak_ptr<DX9GF::IGameObject> parent, float maxHealth, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1) : IGameObject(tm, parent, x, y, rotation, scaleX, scaleY), maxHealth(maxHealth), health(maxHealth) {}
        void InitCardSpawnTrigger(DX9GF::Camera* camera, float width, float height);
		void SetOnRequestEnemyCard(std::function<void(std::shared_ptr<IEnemy>)> callback);
		bool IsDead() const;
		virtual void Update(unsigned long long deltaTime);
		virtual void Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime);
		virtual bool TakeDamage(float damage);
		virtual void StartAttack(std::shared_ptr<Player> player) = 0;
		bool IsDoneAttacking();
	};
}