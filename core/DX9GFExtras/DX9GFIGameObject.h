#pragma once
#include <mutex>
#include <vector>
#include <optional>
namespace DX9GF {
	class IGameObject {
	private:
	protected:
		float absoluteX, absoluteY;
		float relativeX, relativeY;
		std::optional<IGameObject*> parent;
		std::vector<IGameObject> children;
		std::mutex objectMutex;
		void UpdateAbsoluteX();
		void UpdateAbsoluteY();
		void UpdateAbsolutePosition();
		void UpdateRelativeX();
		void UpdateRelativeY();
		void UpdateRelativePosition();
		void UpdateChildren(unsigned long long deltaTime);
		IGameObject() : absoluteX(0), absoluteY(0), relativeX(0), relativeY(0) {}
		IGameObject(float x, float y) : absoluteX(x), absoluteY(y), relativeX(x), relativeY(y) {}
		IGameObject(IGameObject* parent, float x, float y, bool useAbsoluteCoords = false);
		virtual void MainUpdate(unsigned long long deltaTime) = 0;
	public:
		std::vector<IGameObject*> Flatten();
		const std::mutex& GetMutex();
		void SetRelativeX(float x);
		void SetRelativeY(float y);
		void SetRelativePosition(float x, float y);
		void SetAbsoluteX(float x);
		void SetAbsoluteY(float y);
		void SetAbsolutePosition(float x, float y);
		float GetRelativeX() const;
		float GetRelativeY() const;
		/// <summary>
		/// Gets the relative position as a tuple of coordinates.
		/// </summary>
		/// <returns>A tuple containing the x and y coordinates of the relative position.</returns>
		std::tuple<float, float> GetRelativePosition();
		float GetAbsoluteX() const;
		float GetAbsoluteY() const;
		/// <summary>
		/// Gets the absolute position as a tuple of coordinates.
		/// </summary>
		/// <returns>A tuple containing the x and y coordinates of the absolute position.</returns>
		std::tuple<float, float> GetAbsolutePosition();
		void Update(unsigned long long deltaTime);
		virtual void Draw(unsigned long long deltaTime);
		/// <summary>
		/// ALWAYS CALL BASE DISPOSE BEFORE RUNNING YOUR OWN DISPOSE
		/// </summary>
		virtual void Dispose();
	};
};