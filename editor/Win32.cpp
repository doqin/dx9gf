#include "Win32.h"
#include "DX9GF.h"
#include <Windows.h>

std::optional<std::wstring> Win32::OpenFileDialog(const wchar_t* filter)
{
    OPENFILENAME ofn;       // Common dialog box structure
    WCHAR szFile[260] = { 0 }; // Buffer for file name
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = DX9GF::Application::GetInstance()->GetHWnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = static_cast<DWORD>(_countof(szFile));
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL; // Default directory
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    if (GetOpenFileName(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    return std::nullopt;
}
