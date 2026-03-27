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
		D3DPRESENT_PARAMETERS d3dpp;
		bool pendingDeviceReset = false;
		UINT pendingWidth = 0;
		UINT pendingHeight = 0;
		bool TryResetDevice(UINT width, UINT height);
	protected:
		const UINT SCREEN_WIDTH;
		const UINT SCREEN_HEIGHT;

		GraphicsDevice* graphicsDevice;
		SceneManager* sceneManager;
	public:
		IGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight) : hwnd(hwnd), SCREEN_WIDTH(screenWidth), SCREEN_HEIGHT(screenHeight) {}
		virtual ~IGame();

		HWND GetHwnd() const;
		GraphicsDevice* GetGraphicsDevice();
		SceneManager* GetSceneManager();

		virtual void Init();
		virtual void OnResize(UINT width, UINT height);
		void Update(unsigned long long deltaTime);
		void Draw(unsigned long long deltaTime);
		virtual void Dispose();
	};
}