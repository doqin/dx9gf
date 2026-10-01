#pragma once
#include "DX9GFExtras.h"
#include <memory>
#include <string>

namespace Demo {
	// One entry per card background in assets/cardtemplates.png, named by colour since the icon
	// baked into each one is a per-card choice. Values are the template's slot in the sheet - see
	// CardFrame.cpp for the grid. Add a new colour by drawing it in the sheet and appending here.
	enum class CardTemplate {
		Red,
		Magenta,
		Rose,
		Green,
		Forest,
		Cyan,
		Periwinkle,
		Steel,
		Blue,
		Orange,
		Pine,
		Chartreuse,
		Yellow,
		Aqua,
		Sage,
		Count
	};

	// "HeavyStrikeCard" -> "HeavyStrike": the class name minus "Card", PascalCase and unspaced like the
	// hand-drawn strips, and deliberately not translated.
	// ICard::GetDisplayName uses this, which is how a new card gets a face without any art.
	std::wstring CardDisplayNameFromSaveID(const std::string& saveID);

	// Draws a card's face from assets/cardtemplates.png: the template is cut into a left cap
	// (outline + icon), a one-pixel body column and a right cap, the body is stretched to fit the
	// card's name, and the name and energy cost are drawn on top as text. Everything here works in
	// screen pixels; `scale` is how many screen pixels one sheet pixel covers (2 in battle).
	class CardFrame {
	public:
		static constexpr int SHEET_HEIGHT = 16;

		// One frame is shared by every card on a graphics device - it owns the texture, the font and
		// the sprites, none of which are worth building per card. Held weakly, so it goes away with the
		// last card and never outlives the device.
		static std::shared_ptr<CardFrame> Get(DX9GF::GraphicsDevice* graphicsDevice);

		explicit CardFrame(DX9GF::GraphicsDevice* graphicsDevice);

		// Width of a card face carrying `name`. Independent of the cost - room for one digit is always
		// reserved - so a card's width never changes while it is on the table.
		float MeasureWidth(const std::wstring& name, float scale = 2.f);
		static float Height(float scale = 2.f) { return SHEET_HEIGHT * scale; }

		// `x`,`y` is the top-left corner. `tint` multiplies the whole face, text included, which is how
		// the shop greys out cards the player cannot afford.
		void Draw(const DX9GF::Camera& camera, unsigned long long deltaTime, float x, float y,
			CardTemplate cardTemplate, const std::wstring& name, size_t cost,
			float scale = 2.f, D3DCOLOR tint = 0xFFFFFFFF);

	private:
		DX9GF::GraphicsDevice* graphicsDevice;
		std::shared_ptr<DX9GF::Texture> texture;
		std::shared_ptr<DX9GF::StaticSprite> leftCap;
		std::shared_ptr<DX9GF::StaticSprite> body;
		std::shared_ptr<DX9GF::StaticSprite> rightCap;
		std::shared_ptr<DX9GF::StaticSprite> orb;
		// The card font in three cuts of the same metrics: every row (cost digit, measuring), and the
		// rows above / below the card's light-to-dark band split, which the name is drawn in two colours.
		std::shared_ptr<DX9GF::Font> font;
		std::shared_ptr<DX9GF::FontSprite> fontSprite;
		std::shared_ptr<DX9GF::Font> topFont;
		std::shared_ptr<DX9GF::FontSprite> topSprite;
		std::shared_ptr<DX9GF::Font> bottomFont;
		std::shared_ptr<DX9GF::FontSprite> bottomSprite;
		// Width of one digit in the font's native pixels; the cost column is this wide.
		float digitWidth = 0.f;

		void DrawSlice(DX9GF::StaticSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
			RECT src, float x, float y, float scaleX, float scaleY, D3DCOLOR tint);
		void DrawLabel(DX9GF::FontSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
			const std::wstring& text, float x, float y, float scale, D3DCOLOR color, bool outlined, D3DCOLOR tint);
	};
}
