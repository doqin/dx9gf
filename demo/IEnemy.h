#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include "Player.h"
#include "ProjectileSystem.h"
#include <functional>
#include <vector>
#include <deque>
#include "EnemyDialogue.h"
#include "ICombatant.h"
#include "StatusDisplay.h"
namespace Demo {
	class PopUpMessage;

	class IEnemy : public ICombatant {
	protected:
		int goldReward = 0;
		std::vector<std::string> cardDropPool;
		int cardDropMin = 1;
		int cardDropMax = 1;
		struct DamageIndicator {
			std::wstring text;
			float offsetX = 0.f;
			float offsetY = 0.f;
			float vx = 0.f;
			float vy = 0.f;
			unsigned long long elapsed = 0;
			D3DCOLOR textColor = 0xFFFF4444;
		};
		bool isOnStandby = true;
		// "This enemy just debuffed you": a flash + ring on the enemy, an orb flying to the player,
		// an impact ring there, and the debuff's icon and name floating up above the enemy.
		struct DebuffEffect {
			RECT iconRect{ 0, 0, 0, 0 };
			std::wstring name;
			D3DCOLOR color = 0xFFFFFFFF;
			std::weak_ptr<DX9GF::IGameObject> target;
			float targetX = 0.f;
			float targetY = 0.f;
			float elapsed = 0.f;
		};
		std::vector<DebuffEffect> debuffEffects;
		void UpdateDebuffEffects(unsigned long long deltaTime);
		void DrawDebuffEffects(DX9GF::Camera* camera, unsigned long long deltaTime);
		std::function<void(int)> onRequestLockCard = nullptr;
		// Hands a newly created enemy to the battle scene instead of touching its enemy list
		// directly. Spawns run from deferred commands inside the scene's enemy update loop, so
		// inserting straight into that vector would reallocate it mid-iteration.
		std::function<void(std::shared_ptr<IEnemy>)> onRequestSpawnEnemy = nullptr;
		std::function<void(int)> onRequestVisionDebuff = nullptr;
		std::shared_ptr<DX9GF::RectangleTrigger> cardSpawnTrigger;
		std::function<void(std::shared_ptr<IEnemy>)> onRequestEnemyCard = [](std::shared_ptr<IEnemy>) {};
		std::vector<DamageIndicator> damageIndicators;
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<DX9GF::FontSprite> fontSprite;
		ProjectileSystem projectiles;
		DX9GF::CommandBuffer commandBuffer;
		DX9GF::CommandBuffer animationBuffer;
		std::shared_ptr<DX9GF::Texture> uiTexture;
		std::shared_ptr<DX9GF::StaticSprite> uiSprite;
		std::shared_ptr<StatusRenderer> statusRenderer;
		std::shared_ptr<DX9GF::Texture> hitImpactTexture;
		std::vector<std::shared_ptr<DX9GF::AnimatedSprite>> hitImpactSprites;
		DX9GF::GraphicsDevice* graphicsDevice = nullptr;
		unsigned long long timeSinceStart = 0;
		// Speech bubble: lines wait in a queue and are shown one at a time next to the enemy.
		// What each enemy type says lives in EnemyDialogue.
		struct SpeechLine {
			std::wstring text;
			float delayMs = 0.f; // wait before this line shows, e.g. for the battle's transition-in
		};
		static constexpr float SPEECH_SHOW_MS = 2600.f;
		static constexpr float SPEECH_FADE_MS = 180.f;
		static constexpr float SPEECH_MAX_TEXT_W = 230.f;
		static constexpr size_t MAX_QUEUED_SPEECH = 3;
		std::deque<SpeechLine> speechQueue;
		float speechElapsed = 0.f; // how long the front line has been on screen
		bool saidHurtLine = false;
		std::shared_ptr<DX9GF::FontSprite> speechFontSprite;
		void UpdateSpeech(unsigned long long deltaTime);
		// Says the hurt line the first time this enemy drops to half health or below.
		void CheckHurtLine();
		// The speech line EnemyDialogue pairs with this ability popup text, or empty if none.
		std::wstring FindAbilitySpeech(const std::wstring& message) const;
		// Ability lines wait here from OnTurnBegin (where abilities are cast) until the battle scene
		// starts the attack countdown, so the enemy says them right before it attacks. Extra abilities
		// in the same turn only go to the battle log, so the lines fit the countdown.
		static constexpr size_t MAX_ABILITY_LINES_PER_TURN = 1;
		std::vector<std::wstring> pendingAbilityLines;
		void SetGoldReward(int reward) { goldReward = reward; }
		void SetCardDropPool(std::vector<std::string> pool, int minCount = 1, int maxCount = 1) {
			cardDropPool = std::move(pool);
			cardDropMin = minCount;
			cardDropMax = maxCount;
		}
		int lastPattern = -1;
		int streakCount = 0;
		int GetSmartRandomPattern(const int&& minPattern, const int&& maxPattern);

		// Main body sprite, tinted/scaled by the death animation. Null means particles only.
		virtual DX9GF::ISprite* GetBodySprite() { return nullptr; }

		// Programmatic death animation: red flash, fade + shrink, then a burst of particles.
		// IsDoneAttacking() stays false until it finishes so the scene waits before collecting.
		static constexpr float DEATH_FLASH_MS = 150.f;
		static constexpr float DEATH_DURATION_MS = 800.f; // burst lifetime (500ms) after the flash
		bool isDying = false;
		float deathTimer = 0.f;
		float deathBaseScale = 1.f;
		bool deathBurstSpawned = false;
		std::shared_ptr<DX9GF::Texture> deathParticleTexture;
		std::unique_ptr<DX9GF::ParticleSystem> deathParticles;
		void BeginDeath();
		void UpdateDeath(unsigned long long deltaTime);
	public:
		// Class name without namespace, e.g. "SpamEnemy" - the same names EnemyFactory uses.
		std::string GetTypeName() const;
		// This enemy's lines from EnemyDialogue, or null if it has none.
		const EnemyDialogue::Lines* GetDialogue() const;
		// Queues a line for the speech bubble.
		void Say(const std::wstring& text, float delayMs = 0.f);
		void SayIntro(float delayMs = 0.f);
		void SayTaunt();
		// Says the ability lines held since the start of the turn. Called when the attack countdown starts.
		// Says the ability lines held since the start of the turn. Called when the attack countdown
		// starts; returns true if there was anything to say.
		bool SayPendingAbilityLines();
		// A line is on screen or still waiting in the speech queue.
		bool IsSpeaking() const { return !speechQueue.empty(); }
		// Drawn by the battle scene after everything else in the battle area, so nothing covers it.
		void DrawSpeech(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime);

		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, int maxHealth) : ICombatant(tm, maxHealth) { projectiles.SetOwner(this); }

		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, int maxHealth, float x, float y, float rot = 0, float sx = 1, float sy = 1) : ICombatant(tm, maxHealth, x, y, rot, sx, sy) { projectiles.SetOwner(this); }

		IEnemy(std::weak_ptr<DX9GF::TransformManager> tm, std::weak_ptr<DX9GF::IGameObject> parent, int maxHealth, float x, float y, float rot = 0, float sx = 1, float sy = 1) : ICombatant(tm, parent, maxHealth, x, y, rot, sx, sy) { projectiles.SetOwner(this); }
		void InitCardSpawnTrigger(DX9GF::Camera* camera, float width, float height);
		void SetOnRequestEnemyCard(std::function<void(std::shared_ptr<IEnemy>)> callback);
		virtual void Update(unsigned long long deltaTime);
		virtual void Draw(DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera, unsigned long long deltaTime);
		bool TakeDamage(float damage, bool ignoreArmor = false);
		virtual void StartAttack(std::shared_ptr<Player> player, std::vector<std::shared_ptr<IEnemy>>* enemies = nullptr, std::shared_ptr<PopUpMessage> popUpMessage = nullptr, DX9GF::GraphicsDevice* graphicsDevice = nullptr, DX9GF::Camera* camera = nullptr, int currentTurn = 1) = 0;
		void SetState(bool isOnStandby);
		bool IsOnStandby() const { return isOnStandby; }
		bool IsDoneAttacking();
		int GetGoldReward() const { return goldReward; }
		std::vector<std::string> RollCardDrops() const;
		// On-screen size of the sprite, used by the battle scene to pack enemies without
		// overlapping them. Subclasses that draw larger art override these; the default matches
		// the common 64px frame at the 2x scale every enemy sets in Init.
		// Deliberately separate from cardSpawnTrigger, which is the click box.
		virtual float GetBodyWidth() const { return 128.f; }
		virtual float GetBodyHeight() const { return 128.f; }
		std::weak_ptr<DX9GF::RectangleTrigger> GetCardSpawnTrigger() const { return cardSpawnTrigger; }
		void SetOnRequestLockCard(std::function<void(int)> callback) { onRequestLockCard = callback; }
		void SetOnRequestSpawnEnemy(std::function<void(std::shared_ptr<IEnemy>)> callback) { onRequestSpawnEnemy = callback; }
		void SetOnRequestVisionDebuff(std::function<void(int)> callback) { onRequestVisionDebuff = callback; }
		virtual void OnTurnBegin(std::shared_ptr<Player> player, std::shared_ptr<PopUpMessage> popUpMessage, int currentTurn) {}
		virtual void OnTurnBegin(std::shared_ptr<Player> player, std::shared_ptr<PopUpMessage> popUpMessage, int currentTurn, std::vector<std::shared_ptr<IEnemy>>* enemies, DX9GF::GraphicsDevice* graphicsDevice, DX9GF::Camera* camera) {
			OnTurnBegin(player, popUpMessage, currentTurn);
		}

		void OnDebuffInflicted(ICombatant& target, ModifierType type) override;
		bool TakeIndirectDamage(float damage, DamageType type) override;
		void SpawnHealText(int actualHeal) override;
		void CastAbility(std::function<void()> effect, std::shared_ptr<PopUpMessage> popUpMessage, const std::wstring& message);

	protected:
		virtual std::vector<int>& GetPatternOrder() = 0;
		virtual size_t& GetCurrentPatternIndex() = 0;
	};

	template <typename T>
	class EnemyBase : public IEnemy {
	protected:
		std::vector<int>& GetPatternOrder() override {
			static std::vector<int> patternOrder;
			return patternOrder;
		}
		size_t& GetCurrentPatternIndex() override {
			static size_t currentIndex = 0;
			return currentIndex;
		}
	public:
		using IEnemy::IEnemy;
	};
}