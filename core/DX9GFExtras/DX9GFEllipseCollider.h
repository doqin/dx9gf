#pragma once
#include "DX9GFICollider.h"

namespace DX9GF {
	class EllipseCollider : public ICollider {
	private:
		float width;
		float height;
	protected:
	public:
		EllipseCollider(std::weak_ptr<TransformManager> transformManager) : ICollider(transformManager), width(0), height(0) {}
		EllipseCollider(
			std::weak_ptr<TransformManager> transformManager, 
			float width, 
			float height, 
			float x = 0, 
			float y = 0, 
			float rotation = 0, 
			float scaleX = 1, 
			float scaleY = 1
		) : ICollider(transformManager, x, y, rotation, scaleX, scaleY), width(width), height(height) {}
		EllipseCollider(
			std::weak_ptr<TransformManager> transformManager, 
			std::weak_ptr<IGameObject> parent, 
			float width,
			float height,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : ICollider(transformManager, parent, x, y, rotation, scaleX, scaleY), width(width), height(height) { }
		bool IsCollidedEllipse(std::weak_ptr<DX9GF::EllipseCollider> other);
		bool IsCollided(std::weak_ptr<ICollider> other) override;
		std::optional<std::tuple<float, float>> IsIntersecting(std::weak_ptr<ICollider> other, float newX, float newY) override;
		float GetWidth() const;
		float GetHeight() const;
	};
}