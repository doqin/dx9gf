#pragma once
#include "DX9GFExtras.h"
#include "GameItems.h"
#include "CardFrame.h"
#include "CardSpec.h"
#include "CardState.h"
#include "LocalizationManager.h"

namespace Demo {
	class Player;
	class IBattleScene;
	class ICard : virtual public DX9GF::IGameObject, public DX9GF::ISaveable, public CardState {
	protected:
		Player* owner = nullptr;
		IBattleScene* battleScene = nullptr;
		CardSpec spec;
		// Assigned by CardRegistry when the card is created. This is the id stored in saves, so it
		// must keep its value even if the class is renamed.
		std::string saveID;

		// Applies the card's definition. Each concrete card calls it once from its constructor.
		void Configure(CardSpec newSpec) {
			spec = std::move(newSpec);
			SetPersistent(spec.persistent);
			if (spec.maxUses) SetMaxUses(*spec.maxUses);
		}
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

		virtual size_t GetCost() const { return spec.cost; }
		virtual std::wstring GetDescription() const { return spec.description.empty() ? std::wstring() : Tr(spec.description); }
		// Status effects this card inflicts when it executes, and how re-applying them stacks -
		// appended to the tooltip below GetDescription().
		virtual std::vector<AppliedStatusEffect> GetAppliedStatusEffects() const;
		// Which background in assets/cardtemplates.png the card is drawn on.
		virtual CardTemplate GetCardTemplate() const { return spec.face; }
		// Text on the card face. Derived from the save id, so a new card needs no art; override
		// when the id is not what the player should read.
		virtual std::wstring GetDisplayName() const { return CardDisplayNameFromSaveID(GetSaveID()); }
		// How many enemy cards the card takes and how many are attached right now, shown after its
		// name as "(_)" / "(x)". Cards that take no input show "()".
		virtual size_t GetInputSlotCount() const { return 0; }
		virtual size_t GetFilledInputCount() const { return 0; }

		void SetSaveID(std::string id) { saveID = std::move(id); }
		virtual std::string GetSaveID() const override { return saveID; }
		virtual void GenerateSaveData(nlohmann::json& outData) override {}
		virtual void RestoreSaveData(const nlohmann::json& inData) override {}

		virtual void OnDiscard() {}
	};
}
