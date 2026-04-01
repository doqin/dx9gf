#pragma once
#include "ShopScene.h"

namespace Demo {
	class CardShop : public ShopScene {
	public:
		CardShop(Game* game, Player* player, int screenWidth, int screenHeight);


		void LoadItems() override;
	};
}