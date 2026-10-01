#pragma once
#include "DX9GFExtras.h"
#include "GameItems.h"
#include <memory>
#include <optional>
#include <string>

namespace Demo {
	// How one status effect is shown, on the player and on enemies alike. Every status reads the
	// same way: an icon, then its strength as the big number (damage, stacks, charges), then its
	// remaining turns as a small separate "2T". Statuses with no strength - Vulnerable, Weak, Stun
	// and the like - show the icon and the turns only.
	struct StatusView {
		std::wstring name;
		// What it does right now, with the live numbers worked in.
		std::wstring description;
		// The icon in assets/ui.png. Empty for a status with no art, which draws `name` as a label.
		RECT iconRect{ 0, 0, 0, 0 };
		bool hasStrength = false;
		int strength = 0;
		D3DCOLOR color = 0xFFFFFFFF;
		int duration = 0;

		// "Spark (2 turns remaining)" over the description.
		std::wstring Tooltip() const;
	};

	// Nullopt for modifiers that are never shown (an instant heal has no lingering effect to display).
	std::optional<StatusView> DescribeStatus(const CombatModifier& modifier);

	// Draws StatusViews. Shared per graphics device like CardFrame, with its own sprites and fonts,
	// so it can be called while the caller's own sprite batch is open.
	class StatusRenderer {
	public:
		static constexpr float ROW_HEIGHT = 32.f;

		static std::shared_ptr<StatusRenderer> Get(DX9GF::GraphicsDevice* graphicsDevice);
		explicit StatusRenderer(DX9GF::GraphicsDevice* graphicsDevice);

		// `x`,`y` is the row's top-left corner. Returns its width, for the caller's hover hit box.
		float DrawRow(const DX9GF::Camera& camera, unsigned long long deltaTime, float x, float y, const StatusView& status);

	private:
		DX9GF::GraphicsDevice* graphicsDevice;
		std::shared_ptr<DX9GF::Texture> texture;
		std::shared_ptr<DX9GF::StaticSprite> iconSprite;
		// The strength is the main readout in the main font at its 2x size; the turns are in the
		// smaller card font at its native size, so both stay pixel-exact rather than scaled copies.
		std::shared_ptr<DX9GF::Font> bigFont;
		std::shared_ptr<DX9GF::FontSprite> bigSprite;
		std::shared_ptr<DX9GF::Font> smallFont;
		std::shared_ptr<DX9GF::FontSprite> smallSprite;

		float DrawLabel(DX9GF::FontSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
			const std::wstring& text, float x, float y, D3DCOLOR color, float outline);
	};
}
