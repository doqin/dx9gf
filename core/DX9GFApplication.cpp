#include "DX9GFApplication.h"
#include <stdexcept>
#include "DX9GFInputManager.h"
#include <filesystem>

DX9GF::Application* DX9GF::Application::instance = nullptr;
DX9GF::IGame* p_game = nullptr;
std::function<std::optional<LRESULT>(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)> customWndProc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	return std::nullopt;
};

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (auto result = customWndProc(hwnd, msg, wParam, lParam); result.has_value()) {
		return result.value();
	}
	switch (msg)
	{
	case WM_SIZE: {
		UINT w = LOWORD(lParam), h = HIWORD(lParam);
		if (w == 0 || h == 0 || wParam == SIZE_MINIMIZED) return 0;
		if (p_game != nullptr) {
			p_game->OnResize(w, h);
		}
		if (DX9GF::Application::GetInstance() != nullptr) {
			DX9GF::Application::GetInstance()->OnResize(w, h);
		}
		return 0;
	}
	case WM_DESTROY:
		p_game->Dispose();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DX9GF::Application::Init(HINSTANCE hInstance, std::wstring appTitle, UINT screenWidth, UINT screenHeight)
{
	this->hInstance = hInstance;
	this->appTitle = appTitle;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// Very important =)))
	AppRegisterClass();

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
	AdjustWindowRect(&windowRect, WS_VISIBLE | WS_OVERLAPPEDWINDOW, FALSE);

	// Tạo một cửa sổ
	hwnd = CreateWindow(
		appTitle.c_str(),
		appTitle.c_str(),
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left, // chiều rộng
		windowRect.bottom - windowRect.top, // chiều cao
		NULL, // cửa sổ cha
		NULL, // menu
		hInstance, // instance
		NULL // Các tham số cửa sổ
	);

	if (!hwnd) {
		throw std::runtime_error("Error creating window");
	}

	DX9GF::InputManager::GetInstance()->Init(GetHWnd(), hInstance);

	// When launched from Visual Studio, the working directory is often the project folder.
	// When launched from Explorer, it is the output folder (e.g. `example\\x64\\Debug`).
	// The sample uses relative paths like `.\\Resources\\...`, so normalize the working directory.
	std::wstring exePath(MAX_PATH, L'\0');
	DWORD len = GetModuleFileNameW(nullptr, exePath.data(), static_cast<DWORD>(exePath.size()));
	if (len == 0) return;
	exePath.resize(len);

	std::filesystem::path dir = std::filesystem::path(exePath).parent_path();
	std::filesystem::path probe = dir;

	for (int i = 0; i < 8; i++) {
		if (std::filesystem::exists(probe / L"Resources")) {
			SetCurrentDirectoryW(probe.c_str());
			return;
		}
		if (!probe.has_parent_path()) break;
		probe = probe.parent_path();
	}

	// Fallback: at least run from the executable directory.
	SetCurrentDirectoryW(dir.c_str());
}

void DX9GF::Application::SetFramerate(int fps)
{
	frameRate = fps;
}

HWND DX9GF::Application::GetHWnd() const
{
	return hwnd;
}

DX9GF::Application* DX9GF::Application::GetInstance()
{
	if (instance == nullptr) {
		instance = new Application();
	}
	return instance;
}

unsigned int DX9GF::Application::GetScreenWidth() const
{
	return screenWidth;
}

unsigned int DX9GF::Application::GetScreenHeight() const
{
	return screenHeight;
}

void DX9GF::Application::OnResize(UINT width, UINT height)
{
	this->screenWidth = width;
	this->screenHeight = height;
}

void DX9GF::Application::AttachGame(IGame* game)
{
	p_game = game;
}

void DX9GF::Application::SetWndProc(std::function<std::optional<LRESULT>(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)> wndProc)
{
	customWndProc = std::move(wndProc);
}

void DX9GF::Application::Run()
{
	p_game->Init();

	MSG msg;
	int done = 0;
	unsigned long long start = GetTickCount64();
	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			if (frameRate == -1 || GetTickCount64() - start >= 1000 / frameRate) {
				unsigned long long deltaTime = GetTickCount64() - start;
				start = GetTickCount64();
				p_game->Update(deltaTime);
				p_game->Draw(deltaTime);
			}
		}
	}
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
