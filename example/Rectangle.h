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
		void MainUpdate(unsigned long long deltaTime) override;
	public:
		Rectangle(DX9GF::GraphicsDevice* graphicsDevice, float x, float y, float width, float height);
		Rectangle(DX9GF::GraphicsDevice* graphicsDevice, std::weak_ptr<IGameObject> parent, float x, float y, float width, float height, bool useAbsoluteCoords = false);
		void Init();
		void Draw(unsigned long long deltaTime) override;
		void Dispose() override;
	};
}