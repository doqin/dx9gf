#pragma once
#include "DX9GFExtras.h"

namespace Demo {
	class ICard : virtual public DX9GF::IGameObject {
	public:
		inline ICard(std::weak_ptr<DX9GF::TransformManager> transformManager) : IGameObject(transformManager) {}
		inline ICard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) {}
		inline ICard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<DX9GF::IGameObject> parent,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY) {}
		virtual size_t GetCost() const { return 0; }
	};
}
