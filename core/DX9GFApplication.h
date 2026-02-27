#pragma once
#include "framework.h"
#include "DX9GFIGame.h"
#include <string>

namespace DX9GF {
	class Application {
	private:
		HINSTANCE hInstance = nullptr;
		std::wstring appTitle;
		UINT screenWidth = 0;
		UINT screenHeight = 0;

		HWND hwnd = nullptr;
		static Application* instance;
	public:
		/// <summary>
		/// Initialize an Application instance with the specified window parameters.
		/// </summary>
		/// <param name="hInstance">The handle to the application instance provided by Windows.</param>
		/// <param name="appTitle">The title to display in the application window's title bar.</param>
		/// <param name="screenWidth">The width of the application window in pixels.</param>
		/// <param name="screenHeight">The height of the application window in pixels.</param>
		void Init(HINSTANCE hInstance, std::wstring appTitle, UINT screenWidth, UINT screenHeight);
		void AttachGame(IGame* game);
		void Run();

		HWND GetHWnd() const;
		static Application* GetInstance();
		unsigned int GetScreenWidth() const;
		unsigned int GetScreenHeight() const;
	private:
		Application() {}
		~Application() {}
		ATOM AppRegisterClass();
	};
};