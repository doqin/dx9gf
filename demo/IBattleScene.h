#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"
#include "Game.h"
#include "IDraggable.h"
#include "TextButton.h"
#include "MainBlockCard.h"
#include "StrikeCard.h"
#include "EnemyCard.h"
#include "HandContainer.h"
#include "TestEnemy.h"

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
        State lastEnemyLayoutState = State::EnemyAttack;
		bool enemyLayoutInitialized = false;
		bool isExecutingAttacks = false;
		// Externals
		Game* game;
		std::shared_ptr<Player> player;
		std::shared_ptr<Player> battlePlayer;
		// Managers
		DX9GF::CommandBuffer commandBuffer;
		std::shared_ptr<DraggableManager> draggableManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		DX9GF::ColliderManager colliderManager;
       // Battle cards
		std::shared_ptr<MainBlockCard> mainBlockCard;
		std::shared_ptr<HandContainer> handContainer;
		std::vector<std::shared_ptr<IDraggable>> cardHand;
		std::vector<std::shared_ptr<IDraggable>> drawPile;
		std::vector<std::shared_ptr<IDraggable>> playedPile;
		std::vector<std::shared_ptr<IDraggable>> discardPile;
		std::vector<std::shared_ptr<IDraggable>> queuedToDraw;
		size_t currentTurn = 0;
		std::vector<std::shared_ptr<IEnemy>> enemies;
		float enemyCardRemoveAreaX = 220.f;
		float enemyCardRemoveAreaY = -140.f;
		float enemyCardRemoveAreaWidth = 180.f;
		float enemyCardRemoveAreaHeight = 80.f;
		// UI
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<TextButton> attackButton;
		std::shared_ptr<TextButton> actionButton;
		std::shared_ptr<TextButton> itemsButton;
		std::shared_ptr<TextButton> fleeButton;
		std::shared_ptr<TextButton> backButton;
		std::shared_ptr<TextButton> executeButton;
		void CreateEnemyCard(std::shared_ptr<IEnemy> enemy);
		void StartBattle();
		
	private:
		void DrawCards(size_t count);
		void ShuffleDiscardIntoDrawPile();
		void MovePlayedPileToDiscardPileIfNeeded();
		void MoveExecutedHandCardsToPlayedPile();
		void BeginNextTurn();
		// Updates
		void PlayerStandByUpdate(unsigned long long deltaTime);
		void PlayerAttackUpdate(unsigned long long deltaTime);
		void EnemyAttackUpdate(unsigned long long deltaTime);
        void QueueEnemyLayoutTransition(State targetState);
        void RemoveEnemyCardsInRemoveArea();
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