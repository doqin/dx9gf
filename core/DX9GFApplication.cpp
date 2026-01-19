#include "DX9GFApplication.h"

DX9GF::IGame* p_game = NULL;

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		p_game->Dispose();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

std::expected<void, std::wstring> DX9GF::Application::Create()
{
	// Very important =)))
	AppRegisterClass();

	// Tạo một cửa sổ
	hwnd = CreateWindow(
		appTitle.c_str(),
		appTitle.c_str(),
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		screenWidth, // chiều rộng
		screenHeight, // chiều cao
		NULL, // cửa sổ cha
		NULL, // menu
		hInstance, // instance
		NULL // Các tham số cửa sổ
	);

	if (!hwnd) {
		return std::unexpected(L"Error creating window");
	}
	return {};
}

HWND DX9GF::Application::GetHWnd() const
{
	return hwnd;
}

void DX9GF::Application::AttachGame(IGame* game)
{
	p_game = game;
}

std::expected<void, std::wstring> DX9GF::Application::Run()
{
	if (auto result = p_game->Init(); !result.has_value()) {
		return std::unexpected(result.error());
	}

	MSG msg;
	int done = 0;
	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			p_game->Update();
			p_game->Draw();
		}
	}

	return {};
}

ATOM DX9GF::Application::AppRegisterClass()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = DX9GF::Application::appTitle.c_str();
	wc.hIconSm = NULL;

	return RegisterClassEx(&wc);
}
