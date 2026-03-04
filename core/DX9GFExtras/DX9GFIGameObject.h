#pragma once
#include <mutex>
#include <vector>
#include <optional>
#include "DX9GFTransformManager.h"
namespace DX9GF {
	class IGameObject : public std::enable_shared_from_this<IGameObject> {
	private:
	protected:
		TransformHandle transformHandle;
		std::weak_ptr<TransformManager> transformManager;
		std::optional<std::weak_ptr<IGameObject>> parent;
		IGameObject(std::weak_ptr<TransformManager> transformManager);
		IGameObject(std::weak_ptr<TransformManager> transformManager, std::optional<std::weak_ptr<IGameObject>> parent, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1);
	public:
		~IGameObject();
		std::optional<std::weak_ptr<IGameObject>> GetParent() const;
		TransformHandle GetTransformHandle() const;
		void SetLocalX(float x);
		void SetLocalY(float y);
		void SetLocalPosition(float x, float y);
		float GetLocalX() const;
		float GetLocalY() const;
		std::tuple<float, float> GetLocalPosition();
		float GetWorldX() const;
		float GetWorldY() const;
		std::tuple<float, float> GetWorldPosition();
	};
};