#include "pch.h"
#include <Windows.h>
#include "DX9GF.h"
#include "Game.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx9.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	// Get the DX9GF Application
	DX9GF::Application* app = DX9GF::Application::GetInstance();

	// Hard crashes (access violations, driver faults) bypass C++ catch and kill
	// the process silently. Log the exception code, fault details and a stack
	// trace (module + RVA per frame, resolvable against local PDBs) so runs on
	// clean machines (sandbox/CI) are diagnosable instead of a mystery exit
	SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* info) -> LONG {
		std::string report;
		char line[256];

		const EXCEPTION_RECORD* rec = info->ExceptionRecord;
		sprintf_s(line, "Unhandled exception 0x%08X at 0x%p",
			static_cast<UINT>(rec->ExceptionCode), rec->ExceptionAddress);
		report += line;

		// Access violations: say whether it was a read/write and at which address
		if (rec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && rec->NumberParameters >= 2) {
			const char* kind =
				rec->ExceptionInformation[0] == 0 ? "READ" :
				rec->ExceptionInformation[0] == 1 ? "WRITE" : "EXECUTE";
			sprintf_s(line, " (%s of address 0x%IX)", kind, rec->ExceptionInformation[1]);
			report += line;
		}

		// Capture the call stack and resolve each frame to owning module + offset.
		// Offsets are enough to find the faulting function offline using the PDBs
		// from the build machine (demo.pdb / core.pdb)
		void* frames[32] = {};
		const USHORT frameCount = RtlCaptureStackBackTrace(1, 32, frames, nullptr);
		if (frameCount > 0) {
			report += "\r\nStack:\r\n";
			for (USHORT i = 0; i < frameCount; ++i) {
				HMODULE mod = nullptr;
				char modPath[MAX_PATH] = "?";
				DWORD64 rva = reinterpret_cast<DWORD64>(frames[i]);
				if (GetModuleHandleExA(
					GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
					GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
					reinterpret_cast<LPCSTR>(frames[i]), &mod) && mod != nullptr) {
					GetModuleFileNameA(mod, modPath, MAX_PATH);
					const char* fileName = strrchr(modPath, '\\');
					fileName = fileName ? fileName + 1 : modPath;
					rva -= reinterpret_cast<DWORD64>(mod);
					sprintf_s(line, "  %02u  %s+0x%IX", i, fileName, rva);
				}
				else {
					sprintf_s(line, "  %02u  0x%p", i, frames[i]);
				}
				report += line;
				report += "\r\n";
			}
		}

		wchar_t exePath[MAX_PATH]{};
		GetModuleFileNameW(nullptr, exePath, MAX_PATH);
		HANDLE file = CreateFileW(
			(std::wstring(exePath) + L".crash.log").c_str(),
			FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (file != INVALID_HANDLE_VALUE) {
			DWORD written = 0;
			WriteFile(file, report.c_str(), static_cast<DWORD>(report.size()), &written, nullptr);
			WriteFile(file, "\r\n", 2, &written, nullptr);
			CloseHandle(file);
		}
		MessageBoxA(NULL, report.c_str(), "Crash", MB_OK | MB_ICONERROR);
		return EXCEPTION_EXECUTE_HANDLER;
	});

	try {
        app->SetAppIcon(L"assets/icon.ico");
		app->OverrideWindowProc(ImGui_ImplWin32_WndProcHandler);
		app->SetOnDeviceResetHandler([]() {
			ImGui_ImplDX9_InvalidateDeviceObjects();
			ImGui_ImplDX9_CreateDeviceObjects();
		});
		app->Init(hInstance, Demo::GAME_TITLE, 960, 720, false);
		// Create your game that interfaces with IGame
		Demo::Game game(app->GetHWnd(), app->GetScreenWidth(), app->GetScreenHeight());
		game.Init();
		// Attach your game to the application
		app->AttachGame(&game);
		// Run the application
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(app->GetHWnd());
		ImGui_ImplDX9_Init(game.GetGraphicsDevice()->GetDevice());
		app->Run();
	}
	catch (std::exception e) {
		MessageBox(
			NULL,
			std::wstring(e.what(), e.what() + strlen(e.what())).c_str(),
			L"Error", MB_OK | MB_ICONEXCLAMATION
		);
		return E_FAIL;
	}
	// A throw of something that isn't std::exception would otherwise vanish
	// without a trace on machines we can't debug (sandbox/CI/end users)
	catch (...) {
		MessageBox(
			NULL, L"Unknown fatal error (non-standard exception)",
			L"Error", MB_OK | MB_ICONEXCLAMATION
		);
		return E_FAIL;
	}


	return 0;
}