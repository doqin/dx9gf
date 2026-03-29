#pragma once
#include <vector>

namespace Demo {
	class TextInputManager {
	private:
		static TextInputManager* instance;

		TextInputManager();

	public:
		TextInputManager(const TextInputManager&) = delete;
		TextInputManager& operator=(const TextInputManager&) = delete;

		static TextInputManager* GetInstance();
		std::vector<char> ReadInput();
	};
}