#include "pch.h"
#include "SettingsManager.h"
#include <fstream>
#include <cctype> 
#include "DX9GFAudioManager.h"

namespace Demo
{
	SettingsManager* SettingsManager::instance = nullptr;
	SettingsManager::SettingsManager() {}

	void SettingsManager::SetDefaultSettings()
	{
		this->SetMusicVolume(1.0f);
		this->SetSfxVolume(1.0f);
		this->SetMasterVolume(1.0f);

		keybinds["MOVE_LEFT"] = 65; //a
		keybinds["MOVE_UP"] = 87; //w
		keybinds["MOVE_RIGHT"] = 68; //d
		keybinds["MOVE_DOWN"] = 83; //s
	}

	bool SettingsManager::LoadSettings()
	{
		std::ifstream fileInput("config.ini");

		if (!fileInput.is_open())
		{
			this->SetDefaultSettings();
			return true;
		}

		std::string line;

		while (std::getline(fileInput, line))
		{
			//skip empty line
			if (line.empty())
				continue;

			//find '=' to split string
			size_t delimiterPos = line.find('=');
			if (delimiterPos == std::string::npos) continue; //skip line without '='

			std::string key = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);

			//classify data
			if (key == "MASTER_VOL") 
			{
				this->SetMasterVolume(std::stof(value));
			}
			else if (key == "MUSIC_VOL") 
			{
				this->SetMusicVolume(std::stof(value));
			}
			else if (key == "SFX_VOL") 
			{
				this->SetSfxVolume(std::stof(value));
			}
			else 
			{
				this->keybinds[key] = std::stoi(value);
			}
		}

		fileInput.close();
		return true;
	}
	bool SettingsManager::SaveSettings()
	{
		std::ofstream file("config.ini");
		if (!file.is_open()) return false;

		file << "MASTER_VOL=" << masterVolume << "\n";
		file << "MUSIC_VOL=" << musicVolume << "\n";
		file << "SFX_VOL=" << sfxVolume << "\n";

		for (const auto& pair : keybinds)
		{
			file << pair.first << "=" << pair.second << "\n";
		}

		file.close();
		return true;
	}

	void SettingsManager::SetMusicVolume(float vol)
	{
		this->musicVolume = vol;
		DX9GF::AudioManager::GetInstance()->SetMusicVolume(vol); //connect to AudioManager and push vol
	}

	void SettingsManager::SetMasterVolume(float vol)
	{
		this->masterVolume = vol;
		DX9GF::AudioManager::GetInstance()->SetMasterVolume(vol);
	}

	void SettingsManager::SetSfxVolume(float vol)
	{
		this->sfxVolume = vol;
		DX9GF::AudioManager::GetInstance()->SetSfxVolume(vol);
	}

	void SettingsManager::SetKeybind(std::string actionName, int keyCode)
	{
		this->keybinds[actionName] = keyCode;
	}
	int SettingsManager::GetKeybind(std::string actionName)
	{
		// find actionName in keybinds map
		auto it = keybinds.find(actionName);

		if (it != keybinds.end()) 
		{
			return it->second;
		}

		return 0;
	}
}