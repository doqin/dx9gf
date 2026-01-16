#pragma once
#include "framework.h"
#include <string>
#include "DX9GFInterfaces.h"

namespace DX9GF {
	class Application {
	private:
		HINSTANCE hInstance;
		std::wstring appTitle;
		UINT screenWidth;
		UINT screenHeight;

		HWND hwnd = nullptr;
	public:
		Application(HINSTANCE hInstance, std::wstring appTitle, UINT screenWidth, UINT screenHeight) 
			: hInstance(hInstance), 
			appTitle(appTitle), 
			screenWidth(screenWidth), 
			screenHeight(screenHeight) {}

		HRESULT Create();

		HWND GetHWnd() const;

		static void AttachGame(IGame* game);

		HRESULT Run();
	private:
		ATOM AppRegisterClass();
	};
};