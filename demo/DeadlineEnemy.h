#pragma once
#include "IEnemy.h"

namespace Demo {
	class DeadlineEnemy : public EnemyBase<DeadlineEnemy> {
	private:
		std::shared_ptr<DX9GF::Texture> texture;
		std::shared_ptr<DX9GF::AnimatedSprite> sprite;
		std::shared_ptr<DX9GF::Texture> projTexture;
		std::weak_ptr<Player> player;

		int abilityCooldown = 0;

		void PatternCramSession(float projDamage);
		void PatternSubmissionRush(float projDamage);

		static constexpr float FREEZE_VALUE = 0.3f;
		static constexpr int FREEZE_DURATION = 1;

	public:
		using EnemyBase<DeadlineEnemy>::EnemyBase;
		void Init(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera);
		void Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime) override;

		void OnTurnBegin(std::shared_ptr<Player> player, std::shared_ptr<PopUpMessage> popUpMessage, int currentTurn) override;
		void StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies, std::shared_ptr<PopUpMessage> popUpMessage, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, int currentTurn) override;
	};
}