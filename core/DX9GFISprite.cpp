#include "DX9GFISprite.h"

void DX9GF::ISprite::Translate(
	float x,
	float y
)
{
	pos.x += x;
	pos.y += y;
}

void DX9GF::ISprite::SetPosition(float x, float y)
{
	pos.x = x;
	pos.y = y;
}