#pragma once
#include "Windows.h"
#include "DX9GF.h"

namespace Demo{
	class SettingsManager
	{
	private:
		static SettingsManager* instance;
		SettingsManager();

		//Khu vực âm thanh
		float masterVolume;
		float musicVolume;
		float sfxVolume;

		//Khu vực bàn phím
		std::map<std::string, int> keybinds;

		//
		std::string configFilePath;

	public:
		static SettingsManager* GetInstance()
		{
			if (!instance)
			{
				instance = new SettingsManager();
				instance->LoadSettings();
			}
			return instance;
		}

		void SetDefaultSettings();

		bool LoadSettings();
		bool SaveSettings();

		void SetMusicVolume(float vol);
		float GetMusicVolume() { return musicVolume; }

		void SetMasterVolume(float vol);
		float GetMasterVolume() { return masterVolume; }

		void SetSfxVolume(float vol);
		float GetSfxVolume() { return sfxVolume; }

		void SetKeybind(std::string actionName, int keyCode);
		int GetKeybind(std::string actionName);

	};
}
