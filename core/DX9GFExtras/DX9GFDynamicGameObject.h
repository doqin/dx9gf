#pragma once
#include "DX9GFIGameObject.h"
#include <functional>

namespace DX9GF {
	class DynamicGameObject final : public IGameObject {
	private:
		std::unordered_map<std::string, void*> fieldDictionary;
		std::function<void(DynamicGameObject*, unsigned long long)> updateFunction;
		std::function<void(DynamicGameObject*, unsigned long long)> drawFunction;
		std::function<void(DynamicGameObject*)> disposeFunction;
		void MainUpdate(unsigned long long deltaTime) override;
	public:
		DynamicGameObject() : IGameObject() {}
		DynamicGameObject(float x, float y) : IGameObject(x, y) {}
		DynamicGameObject(IGameObject* parent, float x, float y, bool useAbsoluteCoords = false) : IGameObject(parent, x, y, useAbsoluteCoords) {}
		std::unordered_map<std::string, void*>& GetFieldDictionary();
		void SetUpdateFunction(std::function<void(DynamicGameObject*, unsigned long long)> updateFunction);
		void SetDrawFunction(std::function<void(DynamicGameObject*, unsigned long long)> drawFunction);
		void SetDisposeFunction(std::function<void(DynamicGameObject*)> disposeFunction);
		void Init(std::function<void(DynamicGameObject*)> initFunction);
		void Draw(unsigned long long deltaTime) override;
		void Dispose() override;
	};
}