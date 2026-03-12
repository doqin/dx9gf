#include "DX9GFMap.h"
#include "../DX9GFUtils.h"

void DX9GF::Map::Create(std::string pathToTmx)
{
	if (!map.load(pathToTmx)) {
		throw std::invalid_argument("Could not load map file: " + pathToTmx);
	}
	// load the textures
	const auto& tileSets = map.getTilesets();
	assert(!tileSets.empty());
	for (const auto& ts : tileSets)
	{
		textures.emplace_back(std::make_shared<Texture>(graphicsDevice));
		std::wstring imgPath{ std::from_range, ts.getImagePath() };
		textures.back()->LoadTexture(imgPath);
	}
	// load the layers
	const auto& mapLayers = map.getLayers();
	for (auto i = 0u; i < mapLayers.size(); ++i)
	{
		if (mapLayers[i]->getType() == tmx::Layer::Type::Tile)
		{
			layers.emplace_back(std::make_shared<MapLayer>(graphicsDevice));
			layers.back()->Create(this, i); //just cos we're using C++14
		}
	}
}

void DX9GF::Map::Draw(const Camera& camera)
{
	for (auto& layer : layers) {
		layer->Draw(camera);
	}
}

std::vector<std::shared_ptr<DX9GF::Texture>>& DX9GF::Map::GetTextures()
{
	return textures;
}

std::vector<std::shared_ptr<DX9GF::MapLayer>>& DX9GF::Map::GetLayers()
{
	return layers;
}
