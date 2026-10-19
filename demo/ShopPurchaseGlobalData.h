#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <map>
#include <string>

namespace Demo {
	// Save-scoped purchase counters for limited shop rows (#296). A shop scene is rebuilt every time
	// its ShopPoint is opened, so the counts have to live here instead of on the shop - same
	// rationale as SketchyGuyGlobalData.
	class ShopPurchaseGlobalData : public DX9GF::ISaveable {
	private:
		std::map<std::string, int> purchases;

		ShopPurchaseGlobalData() = default;
	public:
		static ShopPurchaseGlobalData* GetInstance() {
			static ShopPurchaseGlobalData instance;
			return &instance;
		}

		void Reset() { purchases.clear(); }

		int GetCount(const std::string& key) const {
			auto it = purchases.find(key);
			return it != purchases.end() ? it->second : 0;
		}
		void AddPurchase(const std::string& key) { purchases[key]++; }

		std::string GetSaveID() const override { return "ShopPurchaseGlobalData"; }
		void GenerateSaveData(nlohmann::json& outData) override {
			outData["purchases"] = purchases;
		}
		void RestoreSaveData(const nlohmann::json& inData) override {
			purchases.clear();
			if (inData.contains("purchases")) {
				purchases = inData["purchases"].get<std::map<std::string, int>>();
			}
		}
	};
}