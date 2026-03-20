#pragma once
#include "IButton.h"
#include <DX9GF.h>
#include "DX9GFTexture.h"
#include <functional>
namespace DX9GF
{
	class cIconButton : public IButton
	{
	private:
		std::shared_ptr<DX9GF::StaticSprite> sprite;
		RECT buttonRects[3];
	public:
		cIconButton(std::shared_ptr<TransformManager> tm,
			float x, float y, float w, float h,
			std::shared_ptr<DX9GF::Texture> uiSheetTex, //contain button sheet
			int startX, int startY, int imgW, int imgH, int spacing, //we need these 5 variables to calculate the button's coordinate
			std::function<void(ITrigger*)> onClick)
			: IButton(tm, x, y, w, h)
		{
			if (uiSheetTex) {
				this->sprite = std::make_shared<DX9GF::StaticSprite>(uiSheetTex.get());
			}

			for (int i = 0; i < 3; ++i) {
				//x_n = x_0 + n * (width + spacing) ->>> Horizontal 
				buttonRects[i].left = startX + i * (imgW + spacing);
				buttonRects[i].top = startY;
				buttonRects[i].right = buttonRects[i].left + imgW;
				buttonRects[i].bottom = buttonRects[i].top + imgH;
			}

			this->SetOnClicked(onClick);
		}

		cIconButton* ChangeSpriteCoords(int startX, int startY, int imgW, int imgH, int spacing);
		void Init(DX9GF::Camera* cam) override;
		void Draw(DX9GF::Camera* camera, DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) override;
		
	};
}