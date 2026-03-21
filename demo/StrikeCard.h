#pragma once
#include "IStatementCard.h"
#include "EnemyCard.h"

namespace Demo {
	class StrikeCard : public IStatementCard {
	private:
		std::weak_ptr<EnemyCard> enemyCard;
		bool isDone = false;
		std::shared_ptr<DX9GF::Font> nameFont;
		std::shared_ptr<DX9GF::FontSprite> nameFontSprite;
	public:
		StrikeCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0) : IStatementCard(tm, 140, 40, x, y) {}
		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		bool Execute() override;
		void ResetExecution() override;
		void Draw(unsigned long long deltaTime) override;
	};
}
