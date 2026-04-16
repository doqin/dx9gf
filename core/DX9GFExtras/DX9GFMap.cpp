#include "pch.h"
#include "DX9GFMap.h"
#include "../DX9GFUtils.h"
#include <filesystem>

void DX9GF::Map::Create(std::weak_ptr<TransformManager> transformManager, std::weak_ptr<ColliderManager> colliderManager, std::string pathToTmx)
{
	if (!std::filesystem::exists(pathToTmx)) {
		throw std::invalid_argument("Could not find map file: " + pathToTmx);
	}
	if (!map.load(pathToTmx)) {
		throw std::invalid_argument("Could not load map file: " + pathToTmx);
	}
	// load the textures
	const auto& tileSets = map.getTilesets();
	assert(!tileSets.empty());
	for (const auto& ts : tileSets)
	{
		std::string imgPathStr = ts.getImagePath();
		if (!std::filesystem::exists(imgPathStr)) {
			// Skip or throw? It's better to throw or log, let's throw.
			throw std::invalid_argument("Could not find texture file: " + imgPathStr);
		}
		textures.emplace_back(std::make_shared<Texture>(graphicsDevice));
		std::wstring imgPath{ std::from_range, imgPathStr };
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
		if (mapLayers[i]->getType() == tmx::Layer::Type::Object)
		{
			const auto& layer = mapLayers[i]->getLayerAs<tmx::ObjectGroup>();
			const auto& objects = layer.getObjects();
			for (auto& object : objects) {
				if (object.getShape() == tmx::Object::Shape::Rectangle) {
					auto rect = object.getAABB();
					std::shared_ptr<RectangleCollider> collider = std::make_shared<RectangleCollider>(transformManager, rect.width, rect.height, rect.left, rect.top);
					colliders.push_back(collider);
					colliderManager.lock()->Add(collider);
				}
			}
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

std::vector<std::shared_ptr<DX9GF::ICollider>>& DX9GF::Map::GetColliders()
{
	return colliders;
}
