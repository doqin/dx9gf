#include "pch.h"
#include "StatusDisplay.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include <cmath>

namespace {
	constexpr float ICON_ADVANCE = 36.f;   // 32px icon plus a gap
	constexpr float TEXT_GAP = 4.f;
	constexpr D3DCOLOR TURNS_COLOR = 0xFFE0E0E0;
	// The turns use the card font (tools/build_cardpixel_font.py), whose capitals are only 6 pixels
	// tall - small next to the main font's, but still at the same pixel scale as the icons and the
	// big number. Its cell is 9 design pixels, so size 18 is 2 screen pixels per design pixel, the
	// scale everything on the row is drawn at. Capitals start one design pixel into the cell and the
	// baseline is 7 design pixels down it.
	constexpr const wchar_t* TURNS_FONT_NAME = L"CardPixel";
	constexpr int TURNS_FONT_SIZE = 18;
	constexpr float TURNS_FONT_BASELINE = 7.f * TURNS_FONT_SIZE / 9.f;
}

std::wstring Demo::StatusView::Tooltip() const {
	return name + L" (" + std::to_wstring(duration) + L" turns remaining)\n" + description;
}

std::optional<Demo::StatusView> Demo::DescribeStatus(const CombatModifier& mod) {
	StatusView view;
	view.duration = mod.duration;
	const int value = static_cast<int>(std::round(mod.value));

	switch (mod.type) {
	case ModifierType::BuffDamage:
		view.name = Tr(L"Atk Up");
		view.description = Tr(L"Increases attack damage.");
		view.iconRect = { 112, 240, 128, 256 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFFfa6a0a;
		break;
	case ModifierType::BuffDefense:
		view.name = Tr(L"Def Up");
		view.description = Tr(L"Blocks incoming damage.");
		view.iconRect = { 96, 240, 112, 256 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFF588dbe;
		break;
	case ModifierType::Poison: {
		// Poison with no value of its own hits for however many turns it has left.
		const int damage = static_cast<int>(std::round(mod.value > 0.f ? mod.value : static_cast<float>(mod.duration)));
		view.name = Tr(L"Poison");
		view.description = Tr(L"Takes ") + std::to_wstring(damage) + Tr(L" damage at end of turn.");
		view.iconRect = { 128, 240, 144, 256 };
		view.hasStrength = true;
		view.strength = damage;
		view.color = 0xFFba4aed;
		break;
	}
	case ModifierType::Burn:
		view.name = Tr(L"Burn");
		view.description = Tr(L"Takes ") + std::to_wstring(value) + Tr(L" damage at end of turn, ignoring block.");
		view.iconRect = { 240, 288, 256, 304 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFFff8800;
		break;
	case ModifierType::Regen:
		view.name = Tr(L"Regen");
		view.description = Tr(L"Heals ") + std::to_wstring(value) + Tr(L" at end of turn.");
		view.iconRect = { 272, 288, 288, 304 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFF9cdb43;
		break;
	case ModifierType::Marked:
		view.name = Tr(L"Marked");
		view.description = Tr(L"Takes ") + std::to_wstring(value) + Tr(L" extra damage from every hit.");
		view.iconRect = { 128, 256, 144, 272 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFFfffc40;
		break;
	case ModifierType::Vulnerable:
		view.name = Tr(L"Vulnerable");
		view.description = Tr(L"Takes 50% more damage from attacks.");
		view.iconRect = { 96, 256, 112, 272 };
		break;
	case ModifierType::Weak:
		view.name = Tr(L"Weak");
		view.description = Tr(L"Deals 25% less damage with attacks.");
		view.iconRect = { 112, 256, 128, 272 };
		break;
	case ModifierType::Stun:
		view.name = Tr(L"Stun");
		view.description = Tr(L"Cannot take action this turn.");
		view.iconRect = { 224, 288, 240, 304 };
		view.color = 0xFFfffc40;
		break;
	case ModifierType::Spark:
		view.name = Tr(L"Spark");
		view.description = Tr(L"Accumulates stacks. Deals no damage until detonated.");
		view.iconRect = { 272, 320, 288, 336 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFFffaa00;
		break;
	case ModifierType::Freeze:
		view.name = Tr(L"Freeze");
		view.description = Tr(L"Player's movement speed is reduced.");
		view.iconRect = { 272, 304, 288, 320 };
		view.color = 0xFF588dbe;
		break;
	case ModifierType::Immunity:
		view.name = Tr(L"Immunity");
		view.description = Tr(L"Blocks debuffs and tick damage.\nLoses 1 charge per block.");
		view.iconRect = { 256, 448, 272, 464 };
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFF00FFFF;
		break;
	case ModifierType::EnergyDrain:
		view.name = Tr(L"Energy Drain");
		view.description = Tr(L"Reduces Energy gained at the start of your turn by ") + std::to_wstring(value) + Tr(L".");
		view.hasStrength = true;
		view.strength = value;
		view.color = 0xFF40c4ff;
		break;
	case ModifierType::InvertedControls:
		view.name = Tr(L"Reversed");
		view.description = Tr(L"Movement is flipped: up<->down, left<->right.");
		view.color = 0xFFff66cc;
		break;
	default:
		return std::nullopt;
	}
	return view;
}

std::shared_ptr<Demo::StatusRenderer> Demo::StatusRenderer::Get(DX9GF::GraphicsDevice* graphicsDevice) {
	static std::weak_ptr<StatusRenderer> cache;
	auto renderer = cache.lock();
	if (!renderer || renderer->graphicsDevice != graphicsDevice) {
		renderer = std::make_shared<StatusRenderer>(graphicsDevice);
		cache = renderer;
	}
	return renderer;
}

Demo::StatusRenderer::StatusRenderer(DX9GF::GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {
	texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	texture->LoadTexture(L"assets/ui.png");
	iconSprite = std::make_shared<DX9GF::StaticSprite>(texture.get());
	iconSprite->SetScale(2.f, 2.f);

	bigFont = std::make_shared<DX9GF::Font>(graphicsDevice, Demo::kMainFontName, Demo::kMainFontSize);
	bigSprite = std::make_shared<DX9GF::FontSprite>(bigFont.get());
	smallFont = std::make_shared<DX9GF::Font>(graphicsDevice, TURNS_FONT_NAME, TURNS_FONT_SIZE);
	smallSprite = std::make_shared<DX9GF::FontSprite>(smallFont.get());
}

float Demo::StatusRenderer::DrawLabel(DX9GF::FontSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
	const std::wstring& text, float x, float y, D3DCOLOR color, float outline) {
	sprite.SetText(text);
	sprite.SetColor(color);
	sprite.SetOutline(true, 0xFF000000, outline);
	sprite.Begin();
	sprite.SetPosition(x, y);
	sprite.Draw(camera, deltaTime);
	sprite.End();
	return static_cast<float>(sprite.GetWidth());
}

float Demo::StatusRenderer::DrawRow(const DX9GF::Camera& camera, unsigned long long deltaTime, float x, float y,
	const StatusView& status) {
	// The big font's cell is shorter than the 32px icon; centre it on the icon.
	const float bigY = y + (ROW_HEIGHT - Demo::kMainFontSize) / 2.f;
	float cursor = x;

	if (status.iconRect.right > status.iconRect.left) {
		iconSprite->SetSrcRect(status.iconRect);
		iconSprite->SetPosition(cursor, y);
		iconSprite->Begin();
		iconSprite->Draw(camera, deltaTime);
		iconSprite->End();
		cursor += ICON_ADVANCE;
	}
	else {
		// No art for this status, so it is named instead.
		cursor += DrawLabel(*bigSprite, camera, deltaTime, status.name, cursor, bigY, status.color, 2.f) + TEXT_GAP * 2.f;
	}

	if (status.hasStrength) {
		cursor += DrawLabel(*bigSprite, camera, deltaTime, std::to_wstring(status.strength), cursor, bigY, status.color, 2.f) + TEXT_GAP;
	}

	// Remaining turns, small and standing on the same baseline as the number. The main font's cell has
	// 10 pixels of ascent per 13, so at its 2x size the baseline is 20 pixels below the cell top.
	const float bigBaseline = bigY + Demo::kMainFontSize * 10.f / Demo::kMainFontCell;
	const float smallY = bigBaseline - TURNS_FONT_BASELINE;
	cursor += DrawLabel(*smallSprite, camera, deltaTime, std::to_wstring(status.duration) + Tr(L"T"), cursor, smallY, TURNS_COLOR, 2.f);
	return cursor - x;
}
