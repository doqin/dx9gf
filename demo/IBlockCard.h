#pragma once
#include "IContainer.h"
#include "IStatementCard.h"

namespace Demo {
	class IBlockCard : public IContainer {
	private:
		size_t executeIndex = 0;
		bool isExecuting = false;
		std::weak_ptr<IStatementCard> currentExecutingCard;
	protected:
		std::vector<std::weak_ptr<IStatementCard>> statementCards;
	public:
		using IContainer::IContainer;
		bool OnDrop(std::shared_ptr<IDraggable> other) override;
		void Update(unsigned long long deltaTime) override;
		void StartExecution();
		void ExecuteIteratively();
		bool IsExecuting() const;
      std::shared_ptr<IStatementCard> GetCurrentExecutingCard() const;
		void ResetExecution();
	};
}
