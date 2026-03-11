#include <Windows.h>
#include "DX9GF.h"
#include "ExampleGame.h"
#include <format>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// Get the DX9GF Application
	DX9GF::Application* app = DX9GF::Application::GetInstance();
	//try {
		app->Init(hInstance, L"Example game", 620, 480);
		// Create your game that interfaces with IGame
		ExampleGame game(app->GetHWnd(), 620, 480);
		// Attach your game to the application
		app->AttachGame(&game);
		// Run the application
		app->Run();
	//}
	//catch (std::exception e) {
	//	MessageBox(
	//		NULL,
	//		std::wstring(e.what(), e.what() + strlen(e.what())).c_str(),
	//		L"Error", MB_OK | MB_ICONEXCLAMATION
	//	);
	//	return E_FAIL;
	//}
	return 0;
}