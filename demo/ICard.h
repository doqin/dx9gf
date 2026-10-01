#pragma once
#include "DX9GFExtras.h"
#include "GameItems.h"
#include "CardFrame.h"

namespace Demo {
	class DraggableManager;
	class Player;
	class IBattleScene;
	class ICard : virtual public DX9GF::IGameObject, public DX9GF::ISaveable {
	protected:
		Player* owner = nullptr;
		IBattleScene* battleScene = nullptr;
		bool isLocked = false;
		int lockedTurns = 0;
		bool isRetained = false;
		// Optional cap on how many times the card may execute in a single battle. Cards that
		// leave this empty are unlimited; once a limited card runs out it is moved to the
		// battle's nullified pile and never returns to the deck for the rest of the battle.
		std::optional<int> maxUses;
		int usesLeft = 0;
		// Persistent cards stay attached to their block and re-execute every turn until the
		// program clears. A non-persistent card is detached at end of turn and returned to the
		// discard pile instead, so it fires once per placement but can be drawn and played again.
		bool isPersistent = true;
	public:
		inline ICard(std::weak_ptr<DX9GF::TransformManager> transformManager) : IGameObject(transformManager) {}
		inline ICard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY) {
		}
		inline ICard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<DX9GF::IGameObject> parent,
			float x,
			float y,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY) {
		}

		void SetOwner(Player* p) { owner = p; }
		Player* GetOwner() const { return owner; }

		void SetBattleScene(IBattleScene* scene) { battleScene = scene; }
		IBattleScene* GetBattleScene() const { return battleScene; }

		// Limited uses. A card without a limit is unlimited and never depletes.
		bool HasLimitedUses() const { return maxUses.has_value(); }
		int GetMaxUses() const { return maxUses.value_or(0); }
		int GetRemainingUses() const { return maxUses.has_value() ? usesLeft : 0; }
		void SetMaxUses(int uses) {
			maxUses = uses;
			usesLeft = uses;
		}
		void ClearMaxUses() {
			maxUses.reset();
			usesLeft = 0;
		}
		void ConsumeUse() {
			if (maxUses.has_value() && usesLeft > 0) {
				--usesLeft;
			}
		}
		bool IsDepleted() const { return maxUses.has_value() && usesLeft <= 0; }
		// Restores the use counter so the card is playable again in the next battle.
		void ResetUses() {
			if (maxUses.has_value()) {
				usesLeft = maxUses.value();
			}
		}

		bool IsPersistent() const { return isPersistent; }
		void SetPersistent(bool persistent) { isPersistent = persistent; }

		virtual size_t GetCost() const { return 0; }
		virtual std::wstring GetDescription() const { return L""; }
		// Status effects this card inflicts when it executes, and how re-applying them stacks -
		// appended to the tooltip below GetDescription(). Empty for cards with no status effect.
		virtual std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const { return {}; }
		// Which background in assets/cardtemplates.png the card is drawn on. The name and energy
		// cost are drawn over it as text by CardFrame, in battle and in the shop alike.
		virtual CardTemplate GetCardTemplate() const { return CardTemplate::Red; }
		// Text on the card face. Derived from the class name, so a new card needs no art; override
		// when the class name is not what the player should read.
		virtual std::wstring GetDisplayName() const { return CardDisplayNameFromSaveID(GetSaveID()); }

		static std::shared_ptr<ICard> CreateCard(
			const std::string& id,
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::shared_ptr<DraggableManager> draggableManager = nullptr,
			DX9GF::GraphicsDevice* graphicsDevice = nullptr,
			DX9GF::Camera* camera = nullptr
		);

		virtual std::string GetSaveID() const override {
			std::string name = typeid(*this).name();
			size_t pos = name.find_last_of(':');
			if (pos != std::string::npos) return name.substr(pos + 1);
			pos = name.find_last_of(' ');
			if (pos != std::string::npos) return name.substr(pos + 1);
			return name;
		}
		virtual void GenerateSaveData(nlohmann::json& outData) override {}
		virtual void RestoreSaveData(const nlohmann::json& inData) override {}
		bool IsLocked() const { return isLocked; }
		int GetLockedTurns() const { return lockedTurns; }
		void SetLocked(int turns) {
			isLocked = true;
			lockedTurns = turns;
		}
		void TickLock() {
			if (isLocked) {
				lockedTurns--;
				if (lockedTurns <= 0) {
					isLocked = false;
					lockedTurns = 0;
				}
			}
		}
		virtual void OnDiscard() {}
		bool IsRetained() const { return isRetained; }
		void SetRetained(bool retain) { isRetained = retain; }
	};
}
