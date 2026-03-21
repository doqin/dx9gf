#pragma once
#include "IDraggable.h"
#include "IEnemy.h"

namespace Demo {
	class IExpressionCard : public IDraggable {
	public:
		using IDraggable::IDraggable;
		virtual std::shared_ptr<IEnemy> GetValue() = 0;
	};
}
