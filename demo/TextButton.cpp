#include "pch.h"
#include "TextButton.h"

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
	this->trigger->SetOnHeldLeft(this->callback);
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