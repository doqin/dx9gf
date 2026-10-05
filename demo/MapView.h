#pragma once
#include "DX9GF.h"
#include "DX9GFExtras.h"
#include <vector>
#include <string>
#include <cstdint>

namespace Demo {
	/// <summary>
	/// Flat-colour overview of a world scene's tile map, drawn as a corner minimap or a
	/// full-screen map. The map is baked once into a coarse grid (one cell per tile) and
	/// only cells the player has been near are shown (fog of war). The fog is saveable.
	/// </summary>
	class MapView {
	public:
		enum class MarkerKind { Player, Chest, Npc, Save, Heal, Shop, Portal };
		struct Marker {
			MarkerKind kind;
			float x, y;           // world position
			bool dimmed = false;  // e.g. an already opened chest
		};

		void Init(DX9GF::Font* font);
		// Bakes the grid from the loaded map. Layers named "background*" are ignored, so
		// decorative sky/water doesn't count as explorable ground.
		void Build(const DX9GF::Map& map);
		bool IsBuilt() const { return built; }

		void Reveal(float worldX, float worldY);

		void DrawMini(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, int virtualWidth, int virtualHeight,
			float playerX, float playerY, const std::vector<Marker>& markers);
		void DrawFull(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, int virtualWidth, int virtualHeight,
			const std::vector<Marker>& markers, const std::wstring& closeKeyName);

		void Save(nlohmann::json& out) const;
		void Restore(const nlohmann::json& in);

	private:
		enum Cell : std::uint8_t { Void = 0, Floor = 1, Wall = 2 };
		struct ClipRect { float x0, y0, x1, y1; };

		bool built = false;
		float tileW = 16.f, tileH = 16.f;
		int originX = 0, originY = 0;  // tile coordinate of grid cell (0,0)
		int cols = 0, rows = 0;
		std::vector<std::uint8_t> cells;
		std::vector<std::uint8_t> visited;
		// Bounding box (grid cells, inclusive) of everything revealed so far
		int visMinX = 0, visMinY = 0, visMaxX = -1, visMaxY = -1;

		std::shared_ptr<DX9GF::FontSprite> fontSprite;

		bool IsVisited(int cx, int cy) const;
		bool WorldToCell(float worldX, float worldY, int& cx, int& cy) const;
		void FillClipped(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float x0, float y0, float x1, float y1,
			D3DCOLOR color, const ClipRect& clip) const;
		// screenOrigin = screen position of the grid's top-left corner, scale = pixels per tile
		void DrawCells(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float screenOriginX, float screenOriginY,
			float scale, const ClipRect& clip) const;
		void DrawMarkers(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float screenOriginX, float screenOriginY,
			float scale, float dotSize, const ClipRect& clip, const std::vector<Marker>& markers) const;
		void DrawText(DX9GF::Camera& uiCamera, const std::wstring& text, float x, float y, D3DCOLOR color);
	};
}
