#include "pch.h"
#include "DX9GFSaveManager.h"
#include <filesystem>

void DX9GF::SaveManager::Clear() { saveables.clear(); }

void DX9GF::SaveManager::Register(ISaveable* saveable)
{
	saveables.push_back(saveable);
}

void DX9GF::SaveManager::Save(const std::string& filepath)
{
	nlohmann::json save;
	for (auto& obj : saveables) {
		nlohmann::json node;
		if (obj) {
			obj->GenerateSaveData(node);
			if (!node.is_null()) {
				save[obj->GetSaveID()] = node;
			}
		}
	}
	std::filesystem::path p(filepath);
	std::filesystem::path dir = p.parent_path();
	if (!dir.empty() && !std::filesystem::exists(dir)) {
		std::error_code ec;
		std::filesystem::create_directories(dir, ec);
	}
	std::ofstream file(filepath);
	if (file.is_open()) {
		file << save.dump(4);
		file.close();
	}
}

void DX9GF::SaveManager::Load(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) return;
	nlohmann::json save;
	file >> save;
	file.close();
	for (auto& obj : saveables) {
		if (obj) {
			std::string id = obj->GetSaveID();
			if (save.contains(id)) {
				obj->RestoreSaveData(save[id]);
			}
		}
	}
}
