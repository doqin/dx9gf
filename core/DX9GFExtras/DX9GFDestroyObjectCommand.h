#pragma once
#include "DX9GFICommand.h"
#include <functional>
#include <memory>
#include <utility>
#include "DX9GFIGameObject.h"

namespace DX9GF {
	class DestroyObjectCommand : public ICommand {
	private:
		std::weak_ptr<IGameObject> gameObject;
		std::function<void(void)> cleanup;
	public:
		DestroyObjectCommand(std::weak_ptr<IGameObject> gameObject, std::function<void(void)> cleanup)
			: gameObject(std::move(gameObject)), cleanup(std::move(cleanup)) {}
		void Execute(unsigned long long deltaTime) override;
	};
}