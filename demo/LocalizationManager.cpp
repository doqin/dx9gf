#include "pch.h"
#include "LocalizationManager.h"
#include "SettingsManager.h"
#include <fstream>

namespace Demo
{
	LocalizationManager* LocalizationManager::instance = nullptr;

	LocalizationManager::LocalizationManager()
	{
		LoadDictionary();
	}

	void LocalizationManager::LoadDictionary()
	{
		std::ifstream file("assets/lang/vi.json");
		if (!file.is_open()) return;

		try {
			nlohmann::json dict;
			file >> dict;

			for (auto& [key, value] : dict.items()) {
				if (value.is_string()) {
					viDictionary[key] = value.get<std::string>();
				}
			}
		}
		catch (const nlohmann::json::exception&) {
			// Malformed dictionary file - fall back to untranslated English everywhere.
		}
	}

	std::wstring LocalizationManager::Translate(const std::wstring& english) const
	{
		if (SettingsManager::GetInstance()->GetLanguage() != Language::VI) {
			return english;
		}

		std::string key = DX9GF::Utils::WideToUtf8(english);
		auto it = viDictionary.find(key);
		if (it == viDictionary.end()) {
			return english;
		}

		return DX9GF::Utils::Utf8ToWide(it->second);
	}

	std::wstring Tr(const std::wstring& english)
	{
		return LocalizationManager::GetInstance()->Translate(english);
	}
}
