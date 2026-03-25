#include "pch.h"
#include "SettingsManager.h"
#include <fstream>
#include <cctype> 
Demo::SettingsManager* Demo::SettingsManager::instance = nullptr;
Demo::SettingsManager::SettingsManager() {}
void Demo::SettingsManager::SetDefaultSettings()
{
	this->musicVolume = 1;
	this->sfxVolume = 1;
	this->masterVolume = 1;

	keybinds["MOVE_LEFT"] = 65; //a
	keybinds["MOVE_UP"] = 87; //w
	keybinds["MOVE_RIGHT"] = 68; //d
	keybinds["MOVE_DOWN"] = 83; //s
}

bool Demo::SettingsManager::LoadSettings()
{
	std::ifstream fileInput("config.ini");

	if (!fileInput.is_open())
	{
		this->SetDefaultSettings();
		return true;
	}

	std::string line;
	//đọc từng dòng

	while (std::getline(fileInput, line))
	{
		// Bỏ qua dòng trống
		if (line.empty()) continue;

		// Tìm vị trí dấu '='
		size_t delimiterPos = line.find('=');
		if (delimiterPos == std::string::npos) continue; // Dòng bị lỗi, không có '=', bỏ qua

		// Cắt đôi chuỗi tại dấu '='
		std::string key = line.substr(0, delimiterPos);
		std::string value = line.substr(delimiterPos + 1);

		// PHÂN LOẠI DATA
		if (key == "MASTER_VOL") {
			this->masterVolume = std::stof(value); // stof = String TO Float
		}
		else if (key == "MUSIC_VOL") {
			this->musicVolume = std::stof(value);
		}
		else if (key == "SFX_VOL") {
			this->sfxVolume = std::stof(value);
		}
		else {
			// Nếu không phải Volume, thì chắc chắn nó là Keybind!
			this->keybinds[key] = std::stoi(value); // stoi = String TO Int
		}
	}

	fileInput.close();
	return true;
}
bool Demo::SettingsManager::SaveSettings()
{
	std::ofstream file;
	file.open("config.ini", std::ios::out);
	if (!file.is_open())
	{
		return false;
	}


	file << "MASTER_VOL=" << masterVolume << "\n";
	file << "MUSIC_VOL=" << musicVolume << "\n";
	file << "SFX_VOL=" << sfxVolume << "\n";

	for (const auto& pair : keybinds) 
	{
		file << pair.first << "=" << pair.second << "\n";
	}
	return true;
}

void Demo::SettingsManager::SetMusicVolume(float vol)
{
	this->musicVolume = vol;
}

void Demo::SettingsManager::SetMasterVolume(float vol)
{
	this->masterVolume = vol;
}

void Demo::SettingsManager::SetSfxVolume(float vol)
{
	this->sfxVolume = vol;
}

void Demo::SettingsManager::SetKeybind(std::string actionName, int keyCode)
{
	this->keybinds[actionName] = keyCode;
}
int Demo::SettingsManager::GetKeybind(std::string actionName)
{
	// Tìm thử xem actionName có trong map không
	auto it = keybinds.find(actionName);

	if (it != keybinds.end()) {
		return it->second; // Tìm thấy -> Trả về mã phím
	}

	// Không tìm thấy (có thể file config bị lỗi) -> Trả về 0 hoặc 1 phím mặc định
	return 0;
}