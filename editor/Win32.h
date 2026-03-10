#pragma once
#include <optional>
#include <string>

namespace Win32 {
	std::optional<std::wstring> OpenFileDialog(const wchar_t* filter = L"All Files\0*.*\0\0");
}