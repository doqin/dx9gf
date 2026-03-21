#pragma once
#include "IButton.h"
#include "DX9GFFont.h"
#include <string>
#include <DX9GF.h>
#include <functional>
namespace Demo
{
	class TextButton : public IButton
	{
	private:
		std::string text;
		std::shared_ptr<DX9GF::FontSprite> fontSprite;

		//save state's color + background
		DWORD idleColor, hoverColor, clickedColor, disabledColor;
		D3DXCOLOR idleBg, hoverBg, clickedBg, disabledBg;

	public:
		TextButton(std::shared_ptr<DX9GF::TransformManager> tm,
			float x, float y, float w, float h,
			std::string txtContent, DX9GF::Font* f,
			std::function<void(DX9GF::ITrigger*)> onClick);

		//setters
		
		TextButton* SetText(std::string newText);
		TextButton* SetBackgroundColors(D3DXCOLOR idle, D3DXCOLOR hover, D3DXCOLOR click, D3DXCOLOR disabled = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.5f));
		TextButton* SetTextColors(DWORD idle, DWORD hover, DWORD click, DWORD disabled = 0xFF888888);

		void Init(DX9GF::Camera* cam) override;
		void Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) override;
	};
}
