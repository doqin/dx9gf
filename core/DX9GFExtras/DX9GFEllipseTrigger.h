#pragma once
#include "DX9GFITrigger.h"
namespace DX9GF {
	class EllipseTrigger : public ITrigger {
	private:
		float width;
		float height;
	protected:
	public:
		EllipseTrigger() : ITrigger(), width(0), height(0) {}
		EllipseTrigger(float width, float height) : width(width), height(height), ITrigger() {}
		EllipseTrigger(float x, float y, float width, float height) : width(width), height(height), ITrigger(x, y) {}
		EllipseTrigger(std::weak_ptr<IGameObject> parent, float x, float y, float width, float height, bool useAbsoluteCoords = false)
			: width(width), height(height), ITrigger(parent, x, y, useAbsoluteCoords) {
		}
		bool IsHovering(unsigned long long deltaTime) override;
	};
}