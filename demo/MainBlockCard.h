#pragma once
#include "IBlockCard.h"

namespace Demo {
	class MainBlockCard : public IBlockCard {
 private:
		std::shared_ptr<DX9GF::Font> nameFont;
		std::shared_ptr<DX9GF::FontSprite> nameFontSprite;
	public:
		MainBlockCard(std::weak_ptr<DX9GF::TransformManager> tm, float x = 0, float y = 0) : IBlockCard(tm, 200, 50, x, y) {}
      void Draw(unsigned long long deltaTime) override;
	};
}
