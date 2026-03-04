#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"

namespace GO {
	class Rectangle : public DX9GF::IGameObject {
	private:
		DX9GF::GraphicsDevice* graphicsDevice;
		std::shared_ptr<DX9GF::RectangleTrigger> trigger;
		float width, height;
	protected:
	public:
		Rectangle(
			DX9GF::GraphicsDevice* graphicsDevice,
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			float width,
			float height,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY),
			width(width),
			height(height),
			graphicsDevice(graphicsDevice) {
		}
		Rectangle(
			DX9GF::GraphicsDevice* graphicsDevice,
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<IGameObject> parent,
			float width,
			float height,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY),
			width(width),
			height(height),
			graphicsDevice(graphicsDevice) {
		}
		void Init();
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		void Dispose();
	};
}