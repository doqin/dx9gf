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

void DX9GF::ISprite::SetPosition(float x, float y, float z)
{
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void DX9GF::ISprite::SetPositionX(float x)
{
	pos.x = x;
}

void DX9GF::ISprite::SetPositionY(float y)
{
	pos.y = y;
}

void DX9GF::ISprite::SetPositionZ(float z)
{
	pos.z = z;
}

float DX9GF::ISprite::GetPositionX() const
{
	return pos.x;
}

float DX9GF::ISprite::GetPositionY() const
{
	return pos.y;
}

float DX9GF::ISprite::GetPositionZ() const
{
	return pos.z;
}

D3DXVECTOR3 DX9GF::ISprite::GetPosition() const
{
	return pos;
}
