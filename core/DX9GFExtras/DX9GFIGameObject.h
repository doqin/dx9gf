#pragma once
#include <mutex>
#include <vector>
#include <optional>
namespace DX9GF {
	class IGameObject : public std::enable_shared_from_this<IGameObject> {
	private:
	protected:
		float absoluteX, absoluteY;
		float relativeX, relativeY;
		std::optional<std::weak_ptr<IGameObject>> parent;
		std::mutex objectMutex;
		void UpdateAbsoluteX();
		void UpdateAbsoluteY();
		void UpdateAbsolutePosition();
		void UpdateRelativeX();
		void UpdateRelativeY();
		void UpdateRelativePosition();
		IGameObject() : absoluteX(0), absoluteY(0), relativeX(0), relativeY(0) {}
		IGameObject(float x, float y) : absoluteX(x), absoluteY(y), relativeX(x), relativeY(y) {}
		IGameObject(std::weak_ptr<IGameObject> parent, float x, float y, bool useAbsoluteCoords = false);
		virtual void MainUpdate(unsigned long long deltaTime);
	public:
		~IGameObject();

		std::mutex& GetMutex();

		std::optional<std::weak_ptr<IGameObject>> GetParent() const;

		void SetRelativeX(float x);
		void SetRelativeY(float y);
		void SetRelativePosition(float x, float y);
		void SetAbsoluteX(float x);
		void SetAbsoluteY(float y);
		void SetAbsolutePosition(float x, float y);
		float GetRelativeX() const;
		float GetRelativeY() const;
		std::tuple<float, float> GetRelativePosition();
		float GetAbsoluteX() const;
		float GetAbsoluteY() const;
		std::tuple<float, float> GetAbsolutePosition();

		void Update(unsigned long long deltaTime);
		virtual void Draw(unsigned long long deltaTime);
		virtual void Dispose();
	};
};