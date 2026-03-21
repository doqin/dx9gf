#include "pch.h"
#include "IconButton.h"
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
	this->trigger->SetOnHeldLeft(this->callback);
}

void Demo::IconButton::Draw(DX9GF::Camera* camera, DX9GF::GraphicsDevice* gd, unsigned long long deltaTime)
{
	int stateIndex = 0; //default is IDLE
	if (this->currentState == ButtonState::HOVER) stateIndex = 1;
	else if (this->currentState == ButtonState::CLICKED) stateIndex = 2;

	if (this->sprite && camera)
	{
		this->sprite->SetSrcRect(this->buttonRects[stateIndex]);

		this->sprite->Begin();
		this->sprite->SetPosition(this->displayX, this->displayY);
		this->sprite->Draw(*camera, deltaTime);
		this->sprite->End();
	}
}
