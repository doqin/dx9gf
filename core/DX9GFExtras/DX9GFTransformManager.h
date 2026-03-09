#pragma once
#include <vector>
#include "DX9GFJobSystem.h"
namespace DX9GF {
	constexpr auto NO_PARENT = -1;

	struct TransformData {
		// Local space (relative to parent)
		float localX = 0, localY = 0;
		float localRotation = 0; // in radian
		float localScaleX = 1, localScaleY = 1;

		// World space (calculated by manager)
		float worldX = 0, worldY = 0;
		float worldRotation = 0;
		float worldScaleX = 1, worldScaleY = 1;

		uint32_t parentSlotIndex = NO_PARENT; // The index of the parent in the sorted array
		uint32_t ownerSlotIndex; // Back-pointer to the stable slot
	};
	struct TransformHandle {
		uint32_t slotIndex; // points to slot index
		uint32_t generation;
	};

	class TransformManager {
	private:
		struct LevelBatch {
			size_t start;
			size_t end;
		};
		struct Slot {
			uint32_t dataIndex; // points to the transforms index (because the order is not stable)
			uint32_t generation = 0;
			bool isAlive = false;
		};

		std::vector<TransformData> transforms; // The actual math data
		std::vector<LevelBatch> levels; // Cached slot indices for parallel dispatch
		std::vector<Slot> slots; // Metadata for handles
		std::vector<uint32_t> freeSlots; // Indices of deleted slots

		void ComputeTransform(int index);
	public:
		void UpdateAll(JobSystem& js);
		void RebuildHierarchy();
		TransformData& GetTransform(size_t index);
		void ValidateSlotIndexing(size_t slotIdx);
		TransformHandle CreateTransform(
			uint32_t parentSlotIdx = NO_PARENT,
			float localX = 0, 
			float localY = 0, 
			float localRotation = 0, 
			float localScaleX = 1, 
			float localScaleY = 1);
		void DestroyTransform(const TransformHandle& handle);
	};
}