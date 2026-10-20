#pragma once
#include "ICard.h"
#include "IContainer.h"
#include "IStatementCard.h"

namespace Demo {
	class IBattleScene;

  class IBlockCard : public ICard, public IContainer {
	private:
		size_t executeIndex = 0;
		bool isExecuting = false;
		std::weak_ptr<IStatementCard> currentExecutingCard;
		float timeSinceLastExecution = 0;
		static constexpr float timePerExecution = .5f;
		float timeSinceLastEnergyPopUp = 999.f;
		static constexpr float energyPopUpCooldown = 3.f;

		// Checks the card can be paid for. When it cannot, warns the player (rate limited) and
		// puts the card back in the hand: dragging detached it, so rejecting the drop would leave
		// it parented to nothing - floating, not executing, yet still swept into the played pile.
		bool AffordOrReturnToHand(const std::shared_ptr<IStatementCard>& card);
		// Drops the card from the execution queue and the container's child list, keeping the
		// execution pointer on the same card. Does not touch the card's parent.
		void RemoveFromQueue(const IStatementCard* card);
		// Queue index a statement card dropped with its centre at dropY would take: after every other
		// queued card whose middle is above that point. Uses the cards' on-screen positions, so a
		// scrolled block still lands right. If slotTopY is given, it receives the top edge of the
		// card currently at that index, or stays untouched when the index is the end of the queue.
		size_t DropIndexAt(float dropY, const std::shared_ptr<IStatementCard>& dropped, float* slotTopY = nullptr);
	protected:
		std::vector<std::weak_ptr<IStatementCard>> statementCards;
		std::shared_ptr<DX9GF::Font> nameFont;
		std::shared_ptr<DX9GF::FontSprite> nameFontSprite;
		std::shared_ptr<DX9GF::Texture> blockTexture;
		std::shared_ptr<DX9GF::StaticSprite> blockSprite;
		std::shared_ptr<DX9GF::StaticSprite> pointerSprite;
		// The block's own header art. The only thing that differs between block kinds - the
		// header, execution pointer and drop highlight below are all shared.
		virtual RECT GetBlockFaceRect() const = 0;
	public:
		inline IBlockCard(std::weak_ptr<DX9GF::TransformManager> transformManager)
			: IGameObject(transformManager), ICard(transformManager), IContainer(transformManager) {}
		inline IBlockCard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			size_t dragAreaWidth,
			size_t dragAreaHeight,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, x, y, rotation, scaleX, scaleY),
			ICard(transformManager, x, y, rotation, scaleX, scaleY),
			IContainer(transformManager, dragAreaWidth, dragAreaHeight, x, y, rotation, scaleX, scaleY) {}
		inline IBlockCard(
			std::weak_ptr<DX9GF::TransformManager> transformManager,
			std::weak_ptr<DX9GF::IGameObject> parent,
			size_t dragAreaWidth,
			size_t dragAreaHeight,
			float x = 0,
			float y = 0,
			float rotation = 0,
			float scaleX = 1,
			float scaleY = 1
		) : IGameObject(transformManager, parent, x, y, rotation, scaleX, scaleY),
			ICard(transformManager, parent, x, y, rotation, scaleX, scaleY),
			IContainer(transformManager, parent, dragAreaWidth, dragAreaHeight, x, y, rotation, scaleX, scaleY) {}
		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		void Update(unsigned long long deltaTime) override;
		void Draw(unsigned long long deltaTime) override;
		void StartExecution();
		// Drops a card from the queue without moving it anywhere. The init block commits its
		// resolved cards straight to a pile; it must forget them the same frame, before Update
		// gets a chance to self-heal the vectors, or end-of-turn cleanup banks them a second time
		// (card shows up duplicated, its cost counted twice).
		void ForgetStatementCard(const std::shared_ptr<IStatementCard>& card);
		void ExecuteIteratively(unsigned long long deltaTime);
		bool IsExecuting() const;
		std::shared_ptr<IStatementCard> GetCurrentExecutingCard() const;
		void ResetExecution();
		bool HasAllRequiredTargets() const;
		// Inserts (or moves, if already attached) a statement card at the given position in the
		// execution queue, shifting the rest as needed (used by keyboard navigation).
		// Returns false if the card can't be attached (e.g. not enough energy for a brand new card).
		bool InsertStatementCardAt(std::shared_ptr<IStatementCard> card, size_t index);
		// World-space position of the slot at the given queue index (0 = first to execute).
		// Pass the card being moved as "excluding" so its own current slot isn't counted twice.
		std::tuple<float, float> GetStatementSlotWorldPosition(size_t index, std::shared_ptr<IStatementCard> excluding = nullptr);
		// Ordered list (execution order) of statement cards currently queued in this block.
		const std::vector<std::weak_ptr<IStatementCard>>& GetStatementCards() const { return statementCards; }
		bool TryReclaim(std::weak_ptr<DX9GF::IGameObject> oldParent) override;
		bool CanBeStoredInContainer() const override { return false; }
	};
}
