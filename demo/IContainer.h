#pragma once
#include "DX9GFExtras.h"
#include "IDraggable.h"

namespace Demo {
	class IContainer : public IDraggable {
	private:
		size_t GetMaxWidthOfChildren();
		size_t GetHeightOfChildren();
	protected:
		std::vector<std::weak_ptr<IDraggable>> children;
		bool isHovered = false;
	public:
		inline IContainer(std::weak_ptr<DX9GF::TransformManager> transformManager) : IDraggable(transformManager) { }
		inline IContainer(
			std::weak_ptr<DX9GF::TransformManager> transformManager, 
			size_t dragAreaWidth, 
			size_t dragAreaHeight,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IDraggable(transformManager, dragAreaWidth, dragAreaHeight, x, y, rotation, scaleX, scaleY) { }
		inline IContainer(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<DX9GF::IGameObject> parent,
			size_t dragAreaWidth,
			size_t dragAreaHeight,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IDraggable(transformManager, parent, dragAreaWidth, dragAreaHeight, x, y, rotation, scaleX, scaleY) { }
		bool OnHover(std::shared_ptr<IDraggable> other) override;
		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
	};
}