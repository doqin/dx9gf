#include <Windows.h>
#include "DX9GF.h"
#include "Editor.h"
#include "imgui/imgui.h"
#include <optional>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	auto app = DX9GF::Application::GetInstance();
	try {
		app->Init(hInstance, L"DX9GF Editor", 1080, 720);
		Editor::Editor editor(app->GetHWnd(), app->GetScreenWidth(), app->GetScreenHeight());
		app->AttachGame(&editor);
		app->SetWndProc([](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> std::optional<LRESULT> {
			if (auto res = ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam); res) {
				return res;
			}
			return std::nullopt;
		});
		app->Run();
	}
	catch (std::exception e) {
		MessageBox(app->GetHWnd(), DX9GF::Utils::ToWide(e.what()).c_str(), L"An error had occurred", MB_OK | MB_ICONERROR);
	}	
}