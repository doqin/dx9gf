#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"

class ExampleGame : public DX9GF::IGame, public DX9GF::ISaveable
{
private:
	DX9GF::SaveManager* saveManager;
	std::unordered_map<std::string, int> sceneMap;
public:
	ExampleGame(HWND hwnd, const UINT screenWidth, const UINT screenHeight, DX9GF::SaveManager* saveManager) : IGame(hwnd, screenWidth, screenHeight), saveManager(saveManager) {};
	void Init() override;

	// Inherited via ISaveable
	std::string GetSaveID() const override;
	void GenerateSaveData(nlohmann::json& outData) override;
	void RestoreSaveData(const nlohmann::json& inData) override;
	DX9GF::SaveManager* GetSaveManager();
};

