#pragma once
#include "DX9GF.h"
#include <memory>
#include <tuple>

namespace Editor {
	class Tileset {
	private:
		struct Equal {
			bool operator()(const std::tuple<float, float>& a, const std::tuple<float, float>& b) const;
		};
		struct Hash {
			size_t operator()(const std::tuple<float, float>& t) const;
		};
	public:
		std::weak_ptr<DX9GF::StaticSprite> spritesheet;
		std::unordered_map<std::tuple<float, float>, RECT, Equal, Hash> clips;
	};
}