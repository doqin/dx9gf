#pragma once
#include "IShopScene.h"

namespace Demo {
	class CardShop : public IShopScene {
	public:
		CardShop(Game* game, Player* player, int screenWidth, int screenHeight);


		void LoadItems() override;
	};
}