#pragma once
#include "framework.h"
#include <d3d9.h>

namespace DX9GF {
	class GraphicsDevice;
	class SceneManager;

	class IGame
	{
	private:
		LPDIRECT3D9 d3d = nullptr; // Biến "tổng" của toàn bộ thư viện Direct3D
		HWND hwnd = NULL;
	protected:
		const UINT SCREEN_WIDTH;
		const UINT SCREEN_HEIGHT;

		GraphicsDevice* graphicsDevice;
		SceneManager* sceneManager;
	public:
		IGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : hwnd(hwnd), SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight) {}
		~IGame();

		HWND GetHwnd() const;
		GraphicsDevice* GetGraphicsDevice();
		SceneManager* GetSceneManager();

		virtual void Init();
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		virtual void Dispose();
	};
}