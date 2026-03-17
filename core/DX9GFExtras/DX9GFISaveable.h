#pragma once
#include <string>
#include "pch.h"

namespace DX9GF {
	class ISaveable {
	public:
		virtual ~ISaveable() = default;
		virtual std::string GetSaveID() const = 0;
		virtual void GenerateSaveData(nlohmann::json& outData) = 0;
		virtual void RestoreSaveData(const nlohmann::json& inData) = 0;
	};
}