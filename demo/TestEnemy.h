#pragma once
#include "IEnemy.h"

namespace Demo {
	class TestEnemy : public IEnemy {
	private:
       std::shared_ptr<DX9GF::Texture> texture;
		std::shared_ptr<DX9GF::StaticSprite> sprite;
	public:
		using IEnemy::IEnemy;
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera);
		void Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime) override;
		std::vector<std::shared_ptr<DX9GF::ICommand>> GetAttackCommand(std::shared_ptr<Player> player, std::function<void(void)> finishAttack) override;
	};
}