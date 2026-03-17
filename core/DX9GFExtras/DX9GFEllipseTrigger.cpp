#include "DX9GFEllipseTrigger.h"
#include "../DX9GFInputManager.h"
#include "../DX9GFUtils.h"
#include <cmath>

namespace {
	constexpr float floatEpsilon = 0.000001f;

	struct Vec2 {
		float x;
		float y;
	};

	Vec2 operator-(const Vec2& a, const Vec2& b) { return { a.x - b.x, a.y - b.y }; }

	Vec2 Rotate(const Vec2& v, float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);
		return { v.x * c - v.y * s, v.x * s + v.y * c };
	}
}

bool DX9GF::EllipseTrigger::IsHovering(unsigned long long deltaTime)
{
    auto input = DX9GF::InputManager::GetInstance();
    float mouseX = input->GetAbsoluteMouseX();
    float mouseY = input->GetAbsoluteMouseY();
    float worldX = GetWorldX();
    float worldY = GetWorldY();
	float rx = width * 0.5f;
	float ry = height * 0.5f;
	if (rx <= floatEpsilon || ry <= floatEpsilon) {
		return false;
	}

	float r = GetWorldRotation();
	float sx = GetWorldScaleX();
	float sy = GetWorldScaleY();
	if (std::abs(sx) <= floatEpsilon || std::abs(sy) <= floatEpsilon) {
		return false;
	}

	Vec2 centerOffset = Rotate({ (rx - originX) * sx, (ry - originY) * sy }, r);
	auto [centerWindowX, centerWindowY] = Utils::WorldToWindowCoords(*camera, worldX + centerOffset.x, worldY + centerOffset.y);

	Vec2 d = { mouseX - centerWindowX, mouseY - centerWindowY };
	Vec2 localScaled = Rotate(d, -r);
	Vec2 local = { localScaled.x / sx, localScaled.y / sy };

	float nx = local.x / rx;
	float ny = local.y / ry;
	return (nx * nx + ny * ny) <= (1.0f + floatEpsilon);
}

void DX9GF::EllipseTrigger::Draw(GraphicsDevice* graphicsDevice, Camera* camera)
{
	if (DX9GF::ITrigger::drawTrigger) {
		graphicsDevice->DrawEllipse(
			*camera,
			GetWorldX(),
			GetWorldY(),
			width,
			height,
			GetWorldRotation(),
			GetWorldScaleX(),
			GetWorldScaleY(),
			GetOriginX(),
			GetOriginY(),
			0x550000FF,
			false
		);
	}

}

void DX9GF::EllipseTrigger::SetOrigin(float x, float y)
{
	originX = x;
	originY = y;
}

void DX9GF::EllipseTrigger::SetOriginCenter()
{
	originX = width * 0.5f;
	originY = height * 0.5f;
}

float DX9GF::EllipseTrigger::GetOriginX() const
{
	return originX;
}

float DX9GF::EllipseTrigger::GetOriginY() const
{
	return originY;
}
