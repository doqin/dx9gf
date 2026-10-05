#include "pch.h"
#include "MapView.h"
#include "LocalizationManager.h"
#include <algorithm>
#include <cmath>

namespace {
	constexpr int kRevealRadiusTiles = 7;

	constexpr D3DCOLOR kFloorColor = 0xFF5B7A99;
	constexpr D3DCOLOR kWallColor = 0xFF2A3347;
	constexpr D3DCOLOR kPanelColor = 0xD0101520;
	constexpr D3DCOLOR kBorderColor = 0xFFE8EEF5;
	constexpr D3DCOLOR kDotOutline = 0xFF000000;

	D3DCOLOR MarkerColor(Demo::MapView::MarkerKind kind, bool dimmed) {
		using K = Demo::MapView::MarkerKind;
		switch (kind) {
		case K::Player: return 0xFFFF4040;
		case K::Chest:  return dimmed ? 0xFF7A6A2A : 0xFFFFD23F;
		case K::Npc:    return 0xFF4FC3F7;
		case K::Save:   return 0xFF66E07A;
		case K::Heal:   return 0xFFFF6BB5;
		case K::Shop:   return 0xFFFFA040;
		case K::Portal: return 0xFFB57BFF;
		}
		return 0xFFFFFFFF;
	}

	const wchar_t* MarkerLabel(Demo::MapView::MarkerKind kind) {
		using K = Demo::MapView::MarkerKind;
		switch (kind) {
		case K::Player: return L"You";
		case K::Chest:  return L"Chest";
		case K::Npc:    return L"NPC";
		case K::Save:   return L"Save point";
		case K::Heal:   return L"Healing point";
		case K::Shop:   return L"Shop";
		case K::Portal: return L"Portal";
		}
		return L"";
	}
}

void Demo::MapView::Init(DX9GF::Font* font)
{
	fontSprite = std::make_shared<DX9GF::FontSprite>(font);
}

void Demo::MapView::Build(const DX9GF::Map& map)
{
	built = false;
	tileW = static_cast<float>((std::max)(1, map.GetTileWidth()));
	tileH = static_cast<float>((std::max)(1, map.GetTileHeight()));

	auto isBackground = [](const std::string& name) { return name.rfind("background", 0) == 0; };

	// Pass 1: gather ground/wall tile coordinates and find the grid bounds.
	std::vector<std::pair<std::int64_t, std::uint8_t>> tiles;
	int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
	for (const auto& layerName : map.GetTileLayerNames()) {
		if (isBackground(layerName)) continue;
		const std::uint8_t type = (layerName == "walls") ? Wall : Floor;
		for (std::int64_t packed : map.GetTileCells(layerName)) {
			int tx, ty;
			DX9GF::Map::UnpackTile(packed, tx, ty);
			minX = (std::min)(minX, tx); maxX = (std::max)(maxX, tx);
			minY = (std::min)(minY, ty); maxY = (std::max)(maxY, ty);
			tiles.emplace_back(packed, type);
		}
	}
	if (tiles.empty()) return;

	originX = minX; originY = minY;
	cols = maxX - minX + 1;
	rows = maxY - minY + 1;
	cells.assign(static_cast<size_t>(cols) * rows, Void);
	visited.assign(cells.size(), 0);
	visMinX = cols; visMinY = rows; visMaxX = -1; visMaxY = -1;

	for (const auto& [packed, type] : tiles) {
		int tx, ty;
		DX9GF::Map::UnpackTile(packed, tx, ty);
		std::uint8_t& cell = cells[static_cast<size_t>(ty - originY) * cols + (tx - originX)];
		cell = (std::max)(cell, type);  // a wall tile wins over ground in the same cell
	}

	// Pass 2: anything blocked by a collision rectangle reads as wall.
	for (const auto& area : map.GetCollisionAreas()) {
		const int x0 = (std::max)(originX, static_cast<int>(std::floor(area.x / tileW)));
		const int y0 = (std::max)(originY, static_cast<int>(std::floor(area.y / tileH)));
		const int x1 = (std::min)(originX + cols - 1, static_cast<int>(std::ceil((area.x + area.width) / tileW)) - 1);
		const int y1 = (std::min)(originY + rows - 1, static_cast<int>(std::ceil((area.y + area.height) / tileH)) - 1);
		for (int ty = y0; ty <= y1; ++ty) {
			for (int tx = x0; tx <= x1; ++tx) {
				std::uint8_t& cell = cells[static_cast<size_t>(ty - originY) * cols + (tx - originX)];
				if (cell == Floor) cell = Wall;
			}
		}
	}

	built = true;
}

bool Demo::MapView::IsVisited(int cx, int cy) const
{
	return cx >= 0 && cy >= 0 && cx < cols && cy < rows && visited[static_cast<size_t>(cy) * cols + cx] != 0;
}

bool Demo::MapView::WorldToCell(float worldX, float worldY, int& cx, int& cy) const
{
	cx = static_cast<int>(std::floor(worldX / tileW)) - originX;
	cy = static_cast<int>(std::floor(worldY / tileH)) - originY;
	return cx >= 0 && cy >= 0 && cx < cols && cy < rows;
}

void Demo::MapView::Reveal(float worldX, float worldY)
{
	if (!built) return;
	const int centerX = static_cast<int>(std::floor(worldX / tileW)) - originX;
	const int centerY = static_cast<int>(std::floor(worldY / tileH)) - originY;
	for (int dy = -kRevealRadiusTiles; dy <= kRevealRadiusTiles; ++dy) {
		for (int dx = -kRevealRadiusTiles; dx <= kRevealRadiusTiles; ++dx) {
			if (dx * dx + dy * dy > kRevealRadiusTiles * kRevealRadiusTiles) continue;
			const int cx = centerX + dx, cy = centerY + dy;
			if (cx < 0 || cy < 0 || cx >= cols || cy >= rows) continue;
			visited[static_cast<size_t>(cy) * cols + cx] = 1;
			visMinX = (std::min)(visMinX, cx); visMaxX = (std::max)(visMaxX, cx);
			visMinY = (std::min)(visMinY, cy); visMaxY = (std::max)(visMaxY, cy);
		}
	}
}

void Demo::MapView::FillClipped(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float x0, float y0, float x1, float y1,
	D3DCOLOR color, const ClipRect& clip) const
{
	x0 = (std::max)(x0, clip.x0); y0 = (std::max)(y0, clip.y0);
	x1 = (std::min)(x1, clip.x1); y1 = (std::min)(y1, clip.y1);
	if (x1 <= x0 || y1 <= y0) return;
	gd->DrawRectangle(uiCamera, x0, y0, x1 - x0, y1 - y0, color, true);
}

void Demo::MapView::DrawCells(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float screenOriginX, float screenOriginY,
	float scale, const ClipRect& clip) const
{
	const int cx0 = (std::max)(0, static_cast<int>(std::floor((clip.x0 - screenOriginX) / scale)));
	const int cx1 = (std::min)(cols - 1, static_cast<int>(std::floor((clip.x1 - screenOriginX) / scale)));
	const int cy0 = (std::max)(0, static_cast<int>(std::floor((clip.y0 - screenOriginY) / scale)));
	const int cy1 = (std::min)(rows - 1, static_cast<int>(std::floor((clip.y1 - screenOriginY) / scale)));

	for (int cy = cy0; cy <= cy1; ++cy) {
		const float y0 = std::round(screenOriginY + cy * scale);
		const float y1 = std::round(screenOriginY + (cy + 1) * scale);
		int cx = cx0;
		while (cx <= cx1) {
			const std::uint8_t type = IsVisited(cx, cy) ? cells[static_cast<size_t>(cy) * cols + cx] : static_cast<std::uint8_t>(Void);
			int runEnd = cx;
			while (runEnd + 1 <= cx1) {
				const std::uint8_t next = IsVisited(runEnd + 1, cy) ? cells[static_cast<size_t>(cy) * cols + runEnd + 1] : static_cast<std::uint8_t>(Void);
				if (next != type) break;
				++runEnd;
			}
			if (type != Void) {
				FillClipped(gd, uiCamera, std::round(screenOriginX + cx * scale), y0,
					std::round(screenOriginX + (runEnd + 1) * scale), y1,
					type == Wall ? kWallColor : kFloorColor, clip);
			}
			cx = runEnd + 1;
		}
	}
}

void Demo::MapView::DrawMarkers(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, float screenOriginX, float screenOriginY,
	float scale, float dotSize, const ClipRect& clip, const std::vector<Marker>& markers) const
{
	// Player last so it is never hidden under another marker
	for (int pass = 0; pass < 2; ++pass) {
		for (const auto& m : markers) {
			if ((m.kind == MarkerKind::Player) != (pass == 1)) continue;
			int cx, cy;
			if (!WorldToCell(m.x, m.y, cx, cy) || !IsVisited(cx, cy)) continue;
			const float sx = std::round(screenOriginX + (m.x / tileW - originX) * scale);
			const float sy = std::round(screenOriginY + (m.y / tileH - originY) * scale);
			const float half = dotSize / 2.f;
			if (sx - half < clip.x0 || sy - half < clip.y0 || sx + half > clip.x1 || sy + half > clip.y1) continue;
			gd->DrawRectangle(uiCamera, sx - half - 1, sy - half - 1, dotSize + 2, dotSize + 2, kDotOutline, true);
			gd->DrawRectangle(uiCamera, sx - half, sy - half, dotSize, dotSize, MarkerColor(m.kind, m.dimmed), true);
		}
	}
}

void Demo::MapView::DrawText(DX9GF::Camera& uiCamera, const std::wstring& text, float x, float y, D3DCOLOR color)
{
	fontSprite->Begin();
	fontSprite->SetColor(color);
	fontSprite->SetPosition(x, y);
	fontSprite->SetText(text);
	fontSprite->Draw(uiCamera, 0);
	fontSprite->End();
}

void Demo::MapView::DrawMini(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, int virtualWidth, int virtualHeight,
	float playerX, float playerY, const std::vector<Marker>& markers)
{
	if (!built) return;
	constexpr float kSize = 128.f;
	constexpr float kMargin = 16.f;
	constexpr float kScale = 4.f;  // pixels per tile
	const float winX = virtualWidth / 2.f - kMargin - kSize;
	const float winY = -virtualHeight / 2.f + kMargin;
	const ClipRect clip{ winX, winY, winX + kSize, winY + kSize };

	gd->SetAlphaBlending(true);
	gd->DrawRectangle(uiCamera, winX - 2, winY - 2, kSize + 4, kSize + 4, kBorderColor, true);
	gd->DrawRectangle(uiCamera, winX, winY, kSize, kSize, kPanelColor, true);

	// Keep the player centred: grid origin sits so the player's tile lands mid-window
	const float playerTileX = playerX / tileW - originX;
	const float playerTileY = playerY / tileH - originY;
	const float screenOriginX = winX + kSize / 2.f - playerTileX * kScale;
	const float screenOriginY = winY + kSize / 2.f - playerTileY * kScale;

	DrawCells(gd, uiCamera, screenOriginX, screenOriginY, kScale, clip);
	DrawMarkers(gd, uiCamera, screenOriginX, screenOriginY, kScale, 4.f, clip, markers);
	gd->SetAlphaBlending(false);
}

void Demo::MapView::DrawFull(DX9GF::GraphicsDevice* gd, DX9GF::Camera& uiCamera, int virtualWidth, int virtualHeight,
	const std::vector<Marker>& markers, const std::wstring& closeKeyName)
{
	const float halfW = virtualWidth / 2.f, halfH = virtualHeight / 2.f;
	gd->SetAlphaBlending(true);
	gd->DrawRectangle(uiCamera, -halfW, -halfH, static_cast<float>(virtualWidth), static_cast<float>(virtualHeight), 0xD0000000, true);

	// Content area leaves room for a title on top and legend + hint at the bottom
	constexpr float kSide = 48.f, kTop = 64.f, kBottom = 128.f;
	const ClipRect area{ -halfW + kSide, -halfH + kTop, halfW - kSide, halfH - kBottom };
	gd->DrawRectangle(uiCamera, area.x0 - 2, area.y0 - 2, area.x1 - area.x0 + 4, area.y1 - area.y0 + 4, kBorderColor, true);
	gd->DrawRectangle(uiCamera, area.x0, area.y0, area.x1 - area.x0, area.y1 - area.y0, kPanelColor, true);

	DrawText(uiCamera, Tr(L"Map"), area.x0, -halfH + 20.f, 0xFFFFFFFF);

	if (built && visMaxX >= visMinX) {
		const float spanW = static_cast<float>(visMaxX - visMinX + 1);
		const float spanH = static_cast<float>(visMaxY - visMinY + 1);
		const float inner = 16.f;  // breathing room inside the panel
		float scale = (std::min)((area.x1 - area.x0 - inner * 2) / spanW, (area.y1 - area.y0 - inner * 2) / spanH);
		scale = scale >= 1.f ? std::floor(scale) : scale;
		scale = (std::min)(scale, 8.f);
		const float screenOriginX = std::round((area.x0 + area.x1) / 2.f - (visMinX + spanW / 2.f) * scale);
		const float screenOriginY = std::round((area.y0 + area.y1) / 2.f - (visMinY + spanH / 2.f) * scale);
		DrawCells(gd, uiCamera, screenOriginX, screenOriginY, scale, area);
		DrawMarkers(gd, uiCamera, screenOriginX, screenOriginY, scale, (std::max)(4.f, (std::min)(scale, 8.f)), area, markers);
	}
	gd->SetAlphaBlending(false);

	// Legend: two rows of colour swatches
	const MarkerKind kinds[] = { MarkerKind::Player, MarkerKind::Chest, MarkerKind::Npc, MarkerKind::Save,
		MarkerKind::Heal, MarkerKind::Shop, MarkerKind::Portal };
	constexpr int kPerRow = 4;
	const float colW = (area.x1 - area.x0) / kPerRow;
	const float legendY = area.y1 + 16.f;
	for (size_t i = 0; i < std::size(kinds); ++i) {
		const float lx = area.x0 + colW * (i % kPerRow);
		const float ly = legendY + 24.f * (i / kPerRow);
		gd->DrawRectangle(uiCamera, lx, ly + 3, 10, 10, kDotOutline, true);
		gd->DrawRectangle(uiCamera, lx + 1, ly + 4, 8, 8, MarkerColor(kinds[i], false), true);
		DrawText(uiCamera, Tr(MarkerLabel(kinds[i])), lx + 18.f, ly, 0xFFFFFFFF);
	}
	DrawText(uiCamera, closeKeyName + L" - " + Tr(L"Close"), area.x0, halfH - 32.f, 0xFFBBBBBB);
}

void Demo::MapView::Save(nlohmann::json& out) const
{
	if (!built) return;
	static const char* kHex = "0123456789abcdef";
	std::string bits;
	bits.reserve(visited.size() / 4 + 1);
	for (size_t i = 0; i < visited.size(); i += 4) {
		int nibble = 0;
		for (size_t b = 0; b < 4 && i + b < visited.size(); ++b) {
			if (visited[i + b]) nibble |= (1 << b);
		}
		bits.push_back(kHex[nibble]);
	}
	out = { {"cols", cols}, {"rows", rows}, {"originX", originX}, {"originY", originY}, {"bits", bits} };
}

void Demo::MapView::Restore(const nlohmann::json& in)
{
	if (!built || !in.is_object()) return;
	// A save from a different map layout can't be mapped onto this grid - start fresh instead
	if (in.value("cols", -1) != cols || in.value("rows", -1) != rows
		|| in.value("originX", INT_MIN) != originX || in.value("originY", INT_MIN) != originY) return;
	const std::string bits = in.value("bits", std::string());
	std::fill(visited.begin(), visited.end(), 0);
	visMinX = cols; visMinY = rows; visMaxX = -1; visMaxY = -1;
	for (size_t i = 0; i < visited.size(); ++i) {
		const size_t charIndex = i / 4;
		if (charIndex >= bits.size()) break;
		const char c = bits[charIndex];
		const int nibble = (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'a' + 10 : 0;
		if (nibble & (1 << (i % 4))) {
			visited[i] = 1;
			const int cx = static_cast<int>(i % cols), cy = static_cast<int>(i / cols);
			visMinX = (std::min)(visMinX, cx); visMaxX = (std::max)(visMaxX, cx);
			visMinY = (std::min)(visMinY, cy); visMaxY = (std::max)(visMaxY, cy);
		}
	}
}
