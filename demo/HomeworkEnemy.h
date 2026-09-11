#pragma once
#include "IEnemy.h"

namespace Demo {
	class HomeworkEnemy : public EnemyBase<HomeworkEnemy> {
	private:
		std::shared_ptr<DX9GF::Texture> texture;
		std::shared_ptr<DX9GF::AnimatedSprite> sprite;
		std::shared_ptr<DX9GF::Texture> projTexture;
		std::weak_ptr<Player> player;

		void PatternPileUp(float projDamage);
		void PatternGroupProject(float projDamage);

		static constexpr float BURN_VALUE = 1.f;
		static constexpr int BURN_DURATION = 1;

	public:
		using EnemyBase<HomeworkEnemy>::EnemyBase;
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera);
		void Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime) override;

		void StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies, std::shared_ptr<PopUpMessage> popUpMessage, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, int currentTurn) override;
	};
}