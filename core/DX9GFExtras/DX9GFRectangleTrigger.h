#pragma once
#include "DX9GFITrigger.h"

namespace DX9GF {
	class RectangleTrigger : public ITrigger {
	private:
		float width;
		float height;
	protected:
	public:
		RectangleTrigger(std::weak_ptr<TransformManager> transformManager) : width(0), height(0), ITrigger(transformManager) {}
		RectangleTrigger(
			std::weak_ptr<TransformManager> transformManager, 
			float width, 
			float height, 
			float x = 0, 
			float y = 0, 
			float rotation = 0, 
			float scaleX = 1, 
			float scaleY = 1
		) : width(width), height(height), ITrigger(transformManager, x, y, rotation, scaleX, scaleY) {}
		RectangleTrigger(
			std::weak_ptr<TransformManager> transformManager, 
			std::weak_ptr<IGameObject> parent, 
			float width, 
			float height, 
			float x = 0, 
			float y = 0, 
			float rotation = 0, 
			float scaleX = 1, 
			float scaleY = 1
		) : width(width), height(height), ITrigger(transformManager, parent, x, y, rotation, scaleX, scaleY) {}
		bool IsHovering(unsigned long long deltaTime) override;
	};
}