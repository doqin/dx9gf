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
		/// <summary>
		/// Constructs an Application instance with the specified window parameters.
		/// </summary>
		/// <param name="hInstance">The handle to the application instance provided by Windows.</param>
		/// <param name="appTitle">The title to display in the application window's title bar.</param>
		/// <param name="screenWidth">The width of the application window in pixels.</param>
		/// <param name="screenHeight">The height of the application window in pixels.</param>
		Application(HINSTANCE hInstance, std::wstring appTitle, UINT screenWidth, UINT screenHeight) 
			: hInstance(hInstance), 
			appTitle(appTitle), 
			screenWidth(screenWidth), 
			screenHeight(screenHeight) {}

		/// <summary>
		/// Initialization.
		/// </summary>
		/// <returns>An expected object containing either success or an error message string on failure.</returns>
		std::expected<void, std::wstring> Create();

		/// <summary>
		/// Gets the window handle associated with this object.
		/// </summary>
		/// <returns>The window handle (HWND) associated with this object.</returns>
		HWND GetHWnd() const;

		/// <summary>
		/// Attaches a game instance to the system.
		/// </summary>
		/// <param name="game">A pointer to the game instance to attach.</param>
		static void AttachGame(IGame* game);

		/// <summary>
		/// Executes the run operation.
		/// </summary>
		/// <returns>An expected object containing either success (void) or an error message string.</returns>
		std::expected<void, std::wstring> Run();
	private:
		ATOM AppRegisterClass();
	};
};