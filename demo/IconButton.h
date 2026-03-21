#pragma once
#include "IButton.h"
#include <DX9GF.h>
#include "DX9GFTexture.h"
#include <functional>
namespace Demo
{
	class IconButton : public IButton
	{
	private:
		std::shared_ptr<DX9GF::StaticSprite> sprite;
		RECT buttonRects[3];
	public:
		IconButton(std::shared_ptr<DX9GF::TransformManager> tm,
			float x, float y, float w, float h,
			std::shared_ptr<DX9GF::Texture> uiSheetTex, //contain button sheet
			int startX, int startY, int imgW, int imgH, int spacing, //we need these 5 variables to calculate the button's coordinate
			std::function<void(DX9GF::ITrigger*)> onClick);

		IconButton* ChangeSpriteCoords(int startX, int startY, int imgW, int imgH, int spacing);
		void Init(DX9GF::Camera* cam) override;
		void Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) override;
		
	};
}