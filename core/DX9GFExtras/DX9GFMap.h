#pragma once
#include <string>
#include <tmxlite/Map.hpp>
#include <vector>
#include <memory>
#include "../DX9GFCamera.h"
#include "../DX9GFTexture.h"
#include "DX9GFMapLayer.h"

namespace DX9GF {
	class Map {
	private:
		GraphicsDevice* graphicsDevice;
		tmx::Map map;
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<std::shared_ptr<MapLayer>> layers;
	public:
		Map(GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {}
		void Create(std::string pathToTmx);
		void Draw(const Camera& camera);
		std::vector<std::shared_ptr<Texture>>& GetTextures();
		std::vector<std::shared_ptr<MapLayer>>& GetLayers();
		friend class MapLayer;
	};
}