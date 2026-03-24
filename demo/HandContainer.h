#pragma once
#include "IContainer.h"
#include "IStatementCard.h"

namespace Demo {
	class HandContainer : public IContainer {
	private:
		std::vector<std::shared_ptr<IDraggable>>* playedPile;
		void Init(std::shared_ptr<DraggableManager> manager, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera) override;
	public:
		HandContainer(std::weak_ptr<DX9GF::TransformManager> tm, size_t width, size_t height, float x = 0, float y = 0)
			: IContainer(tm, width, height, x, y) {
		}
        void Init(std::shared_ptr<DraggableManager> manager, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, std::vector<std::shared_ptr<IDraggable>>* playedPile);
		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		void StoreCard(std::shared_ptr<IDraggable> card);
	};
}
