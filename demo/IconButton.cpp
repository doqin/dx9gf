#include "pch.h"
#include "IconButton.h"
Demo::IconButton::IconButton(std::shared_ptr<DX9GF::TransformManager> tm, float x, float y, float w, float h, std::shared_ptr<DX9GF::Texture> uiSheetTex, int startX, int startY, int imgW, int imgH, int spacing, std::function<void(DX9GF::ITrigger*)> onClick)
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
	this->callback = onClick;
}
Demo::IconButton* Demo::IconButton::ChangeSpriteCoords(int startX, int startY, int imgW, int imgH, int spacing)
{
	//reuse the logic of the constructor
	for (int i = 0; i < 3; ++i) {
		this->buttonRects[i].left = startX + i * (imgW + spacing);
		this->buttonRects[i].top = startY;
		this->buttonRects[i].right = this->buttonRects[i].left + imgW;
		this->buttonRects[i].bottom = this->buttonRects[i].top + imgH;
	}
	return this;
}

void Demo::IconButton::Init(DX9GF::Camera* cam)
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

void Demo::IconButton::Draw(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
{
	int stateIndex = 0; //default is IDLE
	if (this->currentState == ButtonState::HOVER) stateIndex = 1;
	else if (this->currentState == ButtonState::CLICKED) stateIndex = 2;

	if (this->sprite && uiCamera)
	{
		this->sprite->SetPosition(this->GetWorldX(), this->GetWorldY());

		this->sprite->SetSrcRect(this->buttonRects[stateIndex]);

		this->sprite->Begin();
		this->sprite->Draw(*uiCamera, deltaTime);
		this->sprite->End();
	}
}
