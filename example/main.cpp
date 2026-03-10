#include <Windows.h>
#include "DX9GF.h"
#include "ExampleGame.h"
#include <format>
#include <stacktrace>
#include <filesystem>

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// Get the DX9GF Application
	DX9GF::Application* app = DX9GF::Application::GetInstance();
	try {
		app->Init(hInstance, L"Example game", 620, 480);
		// Create your game that interfaces with IGame
		ExampleGame game(app->GetHWnd(), 620, 480);
		// Attach your game to the application
		app->AttachGame(&game);
		// Run the application
		app->Run();
	}
	catch (const std::exception& e) {
		auto st = std::stacktrace::current();
		auto ststr = std::to_string(st);
		MessageBox(
			NULL,
			DX9GF::Utils::ToWide(ststr.c_str()).c_str(),
			DX9GF::Utils::ToWide(e.what()).c_str(),
			MB_OK | MB_ICONEXCLAMATION
		);
		return E_FAIL;
	}
	catch (...) {
		MessageBox(NULL, L"Unknown error", L"Example game", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	return 0;
}