#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Game.h"
#include "Player.h"
#include "SavePoint.h"
#include "InventoryMenu.h"
#include "ShopPoint.h"
#include "HealingPoint.h"
#include "NPC.h"
#include "IConversation.h"
#include "TreasureChestNPC.h"
#include "PopUpMessage.h"
#include "PlayerHUD.h"
#include "MapEnemy.h"
#include "ChapterTitleUI.h"
#include "MapView.h"
#include "TextIconButton.h"

namespace Demo {
	class WorldSceneBase : public DX9GF::IScene, public DX9GF::ISaveable {
	protected:
		bool isGamePaused = false;
		bool isTransitioning = false;
		bool hasSeenChapterIntro = false;

		// Minimap / full map. Portal layers must be registered per scene (RegisterPortalLayer)
		// because not every trigger_* layer is a portal (battle triggers, decoys, secrets).
		MapView mapView;
		std::vector<std::string> portalLayers;
		bool fullMapOpen = false;

		// Fast travel map: the full map with a tab per visited world, opened from a save point.
		// Worlds count as visited once one of their save points has been unlocked.
		bool travelMapOpen = false;
		int travelTab = 0;
		float travelInputGraceMs = 0.f;  // ignores clicks right after opening, so the popup click can't leak through
		std::vector<WorldSceneBase*> travelWorlds;
		MapView::TravelHits travelHits;
		std::string travelHoverId;  // save point highlighted by mouse hover or keyboard selection
		int travelHoverTab = -1;
		void OpenTravelMap();
		void CloseFullMap();
		void UpdateTravelMap(unsigned long long deltaTime);
		void ConfirmTravel(WorldSceneBase* world, const SavePoint& target);

		Game* game;
		std::shared_ptr<DX9GF::ColliderManager> colliderManager;
		std::shared_ptr<DX9GF::TransformManager> transformManager;
		std::shared_ptr<Demo::DraggableManager> draggableManager;
		std::shared_ptr<InventoryMenu> inventoryMenu;
		std::shared_ptr<PlayerHUD> playerHUD;
		std::shared_ptr<DX9GF::SaveManager> saveManager;

		std::vector<std::shared_ptr<SavePoint>> savePoints;
		std::vector<std::shared_ptr<ShopPoint>> shopPoints;
		std::vector<std::shared_ptr<HealingPoint>> healingPoints;
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<DX9GF::Texture> gearTex;
		std::shared_ptr<Player> player;
		std::shared_ptr<DX9GF::Map> map;
		std::shared_ptr<DX9GF::CommandBuffer> drawBuffer;
		std::shared_ptr<DX9GF::CommandBuffer> commandBuffer;
		std::shared_ptr<PopUpMessage> popUpMessage;
		std::wstring chapterTitle = L"Unnamed";
		std::wstring chapterSubtitle = L"< No Subtitle >";
		std::shared_ptr<ChapterTitleUI> chapterTitleUI;
		std::shared_ptr<IConversation> currentConversation;
		std::shared_ptr<INPC> activeNPC = nullptr;
		std::vector<std::shared_ptr<NPC>> mapNPCs;
		std::vector<std::shared_ptr<TreasureChestNPC>> treasureChests;
		std::vector<std::shared_ptr<MapEnemy>> mapEnemies;
		std::function<void()> onConversationEnd;

		WorldSceneBase(Game* game, std::shared_ptr<DX9GF::SaveManager> sm, UINT sw, UINT sh);

		void GenerateSaveData(nlohmann::json& outData) override;
		void RestoreSaveData(const nlohmann::json& inData) override;

		void OpenChestWithDialog(std::shared_ptr<TreasureChestNPC>& chest);

		void SetChapterTitle(const std::wstring& title, const std::wstring& subtitle);

		void RegisterPortalLayer(const std::string& layerName) { portalLayers.push_back(layerName); }

		struct DepthNode {
			float y;
			std::function<void()> drawCall;
			bool operator<(const DepthNode& other) const { return y < other.y; }
		};

		void AddDepthNode(std::vector<DepthNode>& nodes, float y, std::function<void()> drawCall);

		void CreatePortalTransition(int sceneOffset, float targetX, float targetY, const char* bgm = nullptr, float bgmVol = 0.3f);
		void TransitionToScene(int sceneIndex, float targetX, float targetY, const char* bgm = nullptr, float bgmVol = 0.3f, bool fastTravel = false);
		// Called on the destination scene once a fast travel has placed the player, so scenes with
		// per-area state (e.g. the boss world's island) can match it to where the player landed.
		virtual void OnFastTravelArrive(float x, float y) {}

		void SpawnMapEnemy(float x, float y, std::string id, std::vector<std::string> types,
			bool isRand, bool isGlobal, std::function<void(DX9GF::GraphicsDevice*, unsigned long long)> bgDraw,
			int tokenChance = 30, const std::string& questEvent = "", const std::string& questId = "");

		virtual void OnInit() = 0;
		virtual void OnUpdate(unsigned long long deltaTime) {}
		// Hook: a subclass-owned modal (e.g. a terminal UI) that should freeze the
		// world and suppress the shared inventory / interaction input while it is up.
		virtual bool IsSubsceneModalActive() const { return false; }
		// Hook: scenes where the corner minimap makes no sense (e.g. a single small room) return false.
		virtual bool ShowMiniMap() const { return true; }
		virtual void OnDrawWorld(std::vector<DepthNode>& depthNodes, unsigned long long deltaTime) {}
		virtual void OnDrawUI(unsigned long long deltaTime) {}
		virtual void OnGenerateSaveData(nlohmann::json& outData) {}
		virtual void OnRestoreSaveData(const nlohmann::json& inData) {}
		virtual void DrawBackground(DX9GF::GraphicsDevice* gd, unsigned long long deltaTime) = 0;

	public:
		void Init() override;
		void InitCore(float playerX, float playerY, const wchar_t* mapFile);

		void Update(unsigned long long deltaTime) override;
		void DrawWorld(unsigned long long deltaTime) override;
		void DrawUI(unsigned long long deltaTime) override;
		std::shared_ptr<Player> GetPlayer() const { return player; }
		const std::vector<std::shared_ptr<SavePoint>>& GetSavePoints() const { return savePoints; }
		// A walkable position beside the save point, so fast travel never lands in a wall or off the map
		std::pair<float, float> FindSpawnNear(const SavePoint& point) const;
		// includePlayer is false when another world's scene draws this one's map (its player isn't "here")
		std::vector<MapView::Marker> BuildMapMarkers(bool includePlayer = true) const;
	};
}
