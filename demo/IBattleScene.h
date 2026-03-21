#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"
#include "Game.h"
#include "IDraggable.h"
#include "TextButton.h"

namespace Demo {
	class IBattleScene : public DX9GF::IScene {
	protected:
		enum class State {
			PlayerStandBy,
			PlayerAttack,
			EnemyAttack
		};
		// States
		State state = State::PlayerStandBy;
		// Externals
		Game* game;
		std::shared_ptr<Player> player;
		// Managers
		DX9GF::CommandBuffer commandBuffer;
		DraggableManager draggableManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		// UI
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<TextButton> attackButton;
		std::shared_ptr<TextButton> actionButton;
		std::shared_ptr<TextButton> itemsButton;
		std::shared_ptr<TextButton> fleeButton;
		std::shared_ptr<TextButton> backButton;
		
	private:
		// Updates
		void PlayerStandByUpdate(unsigned long long deltaTime);
		void PlayerAttackUpdate(unsigned long long deltaTime);
		virtual void EnemyAttackUpdate(unsigned long long deltaTime) = 0;
		// Draws
		void PlayerStandByDraw(unsigned long long deltaTime);
		void PlayerAttackDraw(unsigned long long deltaTime);
		void EnemyAttackDraw(unsigned long long deltaTime);
	public:
		IBattleScene(Game* game, std::shared_ptr<Player> player, int screenWidth, int screenHeight) : IScene(screenWidth, screenHeight), game(game), player(player) {}
		virtual void Init() override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}