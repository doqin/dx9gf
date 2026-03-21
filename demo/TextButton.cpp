#include "pch.h"
#include "TextButton.h"

Demo::TextButton::TextButton(std::shared_ptr<DX9GF::TransformManager> tm, float x, float y, float w, float h, std::string txtContent, DX9GF::Font* f, std::function<void(DX9GF::ITrigger*)> onClick)
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
	this->callback = onClick;
}

Demo::TextButton* Demo::TextButton::SetText(std::string newText)
{
	this->text = newText;
	return this;
}
Demo::TextButton* Demo::TextButton::SetBackgroundColors(D3DXCOLOR idle, D3DXCOLOR hover, D3DXCOLOR click, D3DXCOLOR disabled)
{
	this->idleBg = idle;
	this->hoverBg = hover;
	this->clickedBg = click;
	this->disabledBg = disabled;
	return this;
}
Demo::TextButton* Demo::TextButton::SetTextColors(DWORD idle, DWORD hover, DWORD click, DWORD disabled)
{
	this->idleColor = idle;
	this->hoverColor = hover;
	this->clickedColor = click;
	this->disabledColor = disabled;
	return this;
}

void Demo::TextButton::Init(DX9GF::Camera* cam)
{
	this->trigger = std::make_shared<DX9GF::RectangleTrigger>
		(
			this->GetTransformManager(), shared_from_this(),
			this->width, this->height, 0, 0, 0, 1, 1
		);

	//lock the trigger
	this->trigger->SetLocalPosition(0, 0);

	this->trigger->Init(cam);
	this->SetOnClicked(this->callback);
	this->uiCamera = cam;
}

void Demo::TextButton::Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
{
	D3DXCOLOR currentBgColor = idleBg;
	DWORD currentTxtColor = idleColor;

	if (this->currentState == ButtonState::HOVER) {
		currentBgColor = hoverBg;
		currentTxtColor = hoverColor;
	}
	else if (this->currentState == ButtonState::CLICKED) {
		currentBgColor = clickedBg;
		currentTxtColor = clickedColor;
	}

	auto currX = this->GetWorldX();
	auto currY = this->GetWorldY();

	//draw button's background
	if (gd && uiCamera) {
		gd->DrawRectangle(
			*uiCamera,
			currX, currY, this->width, this->height,
			0, 1, 1, 0, 0,
			currentBgColor,
			true
		);
	}

	//draw button's text
	std::wstring ws(this->text.begin(), this->text.end());
	if (fontSprite && uiCamera) {
		fontSprite->Begin();
		fontSprite->SetPosition(currX, currY);
		fontSprite->SetColor(currentTxtColor);
		fontSprite->SetText(std::move(ws));
		fontSprite->Draw(*uiCamera, deltaTime);
		fontSprite->End();
	}
}