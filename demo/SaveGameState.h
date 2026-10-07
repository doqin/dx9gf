#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <string>
#include <unordered_map>

namespace Demo {
    class Game;
    class Player;

    class SaveGameState : public DX9GF::ISaveable {
        Game* game;
        std::shared_ptr<DX9GF::SaveManager> saveManager;
        std::unordered_map<std::string, size_t> sceneMap;
        void BuildScenes();
		void ClearScenes();
        // Shared by LoadSavedGame and ReloadLastSave so the load sequence lives in one place.
        void ReloadFromDisk();
    public:
        static constexpr const char* SAVE_FILE = "savegame.json";
        static bool HasSaveFile();
        std::shared_ptr<Player> GetPlayerFromScene(DX9GF::IScene* scene) const;
        int GetSceneIndex(const std::string& saveId) const;

        // Per-world presentation data, shared by loading a save and fast travelling.
        struct SceneInfo {
            const wchar_t* displayName;
            const char* bgm;
            float bgmVolume;
            float bgmFadeSeconds;
        };
        // Returns nullptr for scenes that have no info (intro, lab interior).
        static const SceneInfo* GetSceneInfo(const std::string& saveId);
        SaveGameState(Game* game, std::shared_ptr<DX9GF::SaveManager> saveManager);
        std::string GetSaveID() const override;
        void GenerateSaveData(nlohmann::json& outData) override;
        void RestoreSaveData(const nlohmann::json& inData) override;
        void ReloadLastSave();
        static std::shared_ptr<SaveGameState> StartNewGame(Game* game, const std::shared_ptr<DX9GF::SaveManager>& saveManager);
        static std::shared_ptr<SaveGameState> LoadSavedGame(Game* game, const std::shared_ptr<DX9GF::SaveManager>& saveManager);
    };
}
