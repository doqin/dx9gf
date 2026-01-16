#include <Windows.h>
#include "DX9GFApplication.h"
#include "ExampleGame.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// Create the DX9GF Application
	DX9GF::Application app(hInstance, L"Example game", 620, 480);
	app.Create();

	// Create your game that interfaces with IGame
	ExampleGame game(app.GetHWnd(), 620, 480);

	// Attach your game to the application
	DX9GF::Application::AttachGame(&game);

	// Run the app
	return app.Run();
}