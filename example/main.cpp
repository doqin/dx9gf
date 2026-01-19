#include <Windows.h>
#include "DX9GFApplication.h"
#include "ExampleGame.h"
#include <format>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// Create the DX9GF Application
	DX9GF::Application app(hInstance, L"Example game", 620, 480);
	auto result = app.Create();
	if (!result.has_value()) {
		MessageBox(
			NULL, 
			std::format(L"Error creating window: {}", result.error()).c_str(), 
			L"Error", MB_OK | MB_ICONEXCLAMATION
		);
		return E_FAIL;
	}

	// Create your game that interfaces with IGame
	ExampleGame game(app.GetHWnd(), 620, 480);

	// Attach your game to the application
	DX9GF::Application::AttachGame(&game);

	// Run the app
	result = app.Run();
	if (!result.has_value()) {
		MessageBox(
			NULL,
			std::format(L"Error creating window: {}", result.error()).c_str(),
			L"Error", MB_OK | MB_ICONEXCLAMATION
		);
		return E_FAIL;
	}
	return 0;
}