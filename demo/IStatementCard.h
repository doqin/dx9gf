#pragma once
#include "IDraggable.h"

namespace Demo {
	class IStatementCard : public IDraggable {
	public:
		using IDraggable::IDraggable;
		virtual bool Execute() = 0;
		virtual void ResetExecution() {}
	};
}
