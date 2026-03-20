#pragma once
#include "IButton.h"
#include "DX9GFFont.h"
#include <string>
#include <DX9GF.h>
#include <functional>
namespace DX9GF
{
	class cTextButton : public IButton
	{
	private:
		std::string text;
		std::shared_ptr<DX9GF::FontSprite> fontSprite;

		//save state's color + background
		DWORD idleColor, hoverColor, clickedColor, disabledColor;
		D3DXCOLOR idleBg, hoverBg, clickedBg, disabledBg;

	public:
		cTextButton(std::shared_ptr<TransformManager> tm,
			float x, float y, float w, float h,
			std::string txtContent, Font* f,
			std::function<void(ITrigger*)> onClick)
			: IButton(tm, x, y, w, h)
		{

			this->text = txtContent;
			this->fontSprite = std::make_shared<DX9GF::FontSprite>(f);

			//setup text color (HEX)
			this->idleColor = 0xFFFFFFFF; //white
			this->hoverColor = 0xFFFFFF00; //yellow 
			this->clickedColor = 0xFFFF0000; //red
			this->disabledColor = 0xFF888888; //grey

			//setup background color (RGBA)
			this->idleBg = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);   //dark grey
			this->hoverBg = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);  //light grey
			this->clickedBg = D3DXCOLOR(0.1f, 0.8f, 0.1f, 1.0f); //green
			this->disabledBg = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.5f);

			//save the behavior to use on Init (->trigger)
			this->SetOnClicked(onClick);
		}

		//setters
		cTextButton* SetText(std::string newText);
		cTextButton* SetBackgroundColors(D3DXCOLOR idle, D3DXCOLOR hover, D3DXCOLOR click, D3DXCOLOR disabled = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.5f));
		cTextButton* SetTextColors(DWORD idle, DWORD hover, DWORD click, DWORD disabled = 0xFF888888);

		void Init(DX9GF::Camera* cam) override;
		void Draw(DX9GF::Camera* camera, DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) override;
	};
}
