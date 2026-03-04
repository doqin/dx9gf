#pragma once
#include <vector>

namespace DX9GF {
	class IScene;

	class SceneManager final {
	private:
		std::vector<IScene*> scenes;
	protected:
		void Dispose();
	public:
		~SceneManager();
		void ChangeScene(IScene*);
		void PushScene(IScene*);
		void PopScene();
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		void OnResize(int width, int height);
	};
}