#pragma once
#include "DX9GF.h"
#include <string>
#include <unordered_map>

namespace Demo
{
	class LocalizationManager
	{
	private:
		static LocalizationManager* instance;
		LocalizationManager();

		std::unordered_map<std::string, std::string> viDictionary; // English (UTF-8) -> Vietnamese (UTF-8)

		void LoadDictionary();
	public:
		static LocalizationManager* GetInstance()
		{
			if (!instance)
			{
				instance = new LocalizationManager();
			}
			return instance;
		}

		// Translates an English wide string literal to the currently active
		// language (per SettingsManager::GetLanguage()). Falls back to the
		// original English text when no translation is loaded/found.
		std::wstring Translate(const std::wstring& english) const;
	};

	// Convenience wrapper: Tr(L"Continue") reads better at call sites than
	// LocalizationManager::GetInstance()->Translate(L"Continue").
	std::wstring Tr(const std::wstring& english);
}
