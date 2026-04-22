#pragma once
#include <vector>
#include "DX9GFISaveable.h"
#include <memory>

namespace DX9GF {
	class SaveManager {
	private:
		std::vector<ISaveable*> saveables;
	public:
		void Clear();
		void Register(ISaveable* saveable);
		void Save(const std::string& filepath);
		void Load(const std::string& filepath);
	};
}