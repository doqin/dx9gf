#pragma once
#include <mutex>
#include <vector>
#include <optional>
#include "DX9GFTransformManager.h"
namespace DX9GF {
	class IGameObject : public std::enable_shared_from_this<IGameObject> {
	public:
		enum class State {
			Active,
			Inactive,
			Destroyed
		};
	private:
	protected:
		State state = State::Active;
		TransformHandle transformHandle;
		std::weak_ptr<TransformManager> transformManager;
		std::optional<std::weak_ptr<IGameObject>> parent;
		IGameObject(std::weak_ptr<TransformManager> transformManager);
		IGameObject(std::weak_ptr<TransformManager> transformManager, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1);
		IGameObject(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<IGameObject> parent, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1);
	public:
		virtual ~IGameObject();
		bool operator==(const IGameObject& other) const;
		std::optional<std::weak_ptr<IGameObject>> GetParent() const;
		TransformHandle GetTransformHandle() const;
		std::weak_ptr<TransformManager> GetTransformManager();
		// Very dangerous, use when you know what you're doing =)
		void SetTransformHandle(TransformHandle handle);
		void SetTransformManager(std::weak_ptr<TransformManager> transformManager);
		TransformHandle CreateTransform();
		TransformHandle CreateTransform(float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1);
		TransformHandle CreateTransform(std::weak_ptr<IGameObject> parent, float x, float y, float rotation = 0, float scaleX = 1, float scaleY = 1);
		// Transforms
		void SetLocalX(float x);
		void SetLocalY(float y);
		void SetLocalPosition(float x, float y);
		void SetLocalRotation(float r);
		void SetLocalScale(float sx, float sy);
		void SetLocalScaleX(float sx);
		void SetLocalScaleY(float sy);
		float GetLocalX() const;
		float GetLocalY() const;
		std::tuple<float, float> GetLocalPosition();
		float GetWorldX() const;
		float GetWorldY() const;
		std::tuple<float, float> GetWorldPosition();
		float GetLocalRotation() const;
		float GetWorldRotation() const;
		std::tuple<float, float> GetLocalScale();
		float GetLocalScaleX();
		float GetLocalScaleY();
		std::tuple<float, float> GetWorldScale();
		float GetWorldScaleX();
		float GetWorldScaleY();
		State GetState() const;
		friend class ICommand;
	};
};