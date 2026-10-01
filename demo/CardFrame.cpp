#include "pch.h"
#include "CardFrame.h"
#include "MainFont.h"
#include "LocalizationManager.h"
#include "DX9GFUtils.h"
#include <algorithm>
#include <cmath>

namespace {
	// assets/cardtemplates.png: 48x16 cards in two columns. The left column holds 8 templates from
	// the top; the right column's first slot is the energy orb, so its templates start one row down.
	constexpr int TEMPLATE_W = 48;
	constexpr int TEMPLATE_H = 16;
	constexpr int LEFT_COLUMN_X = 0;
	constexpr int RIGHT_COLUMN_X = 48;
	constexpr int LEFT_COLUMN_COUNT = 8;

	// Cut points inside one template. Everything from x=11 to x=44 is the same column repeated (the
	// icon ends by x=11), so any column in that stretch can be the body; 16 stays clear of the blue
	// template's grid lines.
	constexpr int LEFT_CAP_W = 14;
	constexpr int RIGHT_CAP_W = 3;
	constexpr int BODY_SRC_X = 16;

	constexpr RECT ORB_SRC = { 52, 4, 60, 12 };
	constexpr int ORB_SIZE = 8;

	// Spacing, in sheet pixels.
	constexpr int NAME_COST_GAP = 3;
	constexpr int DIGIT_ORB_GAP = 1;
	constexpr int ORB_RIGHT_GAP = 1;

	// tools/build_cardpixel_font.py builds this font from the hand-drawn lettering the card sprites
	// used to have. Its cell is 9 design pixels, so size 18 is 2 screen pixels per design pixel -
	// its look at the battle scale of 2. Other scales stretch it by scale / FONT_NATIVE_SCALE.
	constexpr const wchar_t* CARD_FONT_NAME = L"CardPixel";
	constexpr const wchar_t* CARD_FONT_TOP_NAME = L"CardPixelTop";
	constexpr const wchar_t* CARD_FONT_BOTTOM_NAME = L"CardPixelBottom";
	constexpr int CARD_FONT_SIZE = 18;
	constexpr float FONT_NATIVE_SCALE = 2.f;
	// Where the font's cell starts, in sheet pixels from the top of the card. Capitals begin one
	// design pixel into the cell, so this puts them on rows 5-10 of the 16-row card like the old art.
	constexpr int TEXT_TOP = 4;

	// Name colours lifted from the hand-drawn strips this replaced, indexed like CardTemplate: the
	// upper half of the letters over the card's light band, the lower half over its dark one. The
	// cost digit was the same off-white, outlined in black, on every card.
	struct NameColors { D3DCOLOR top; D3DCOLOR bottom; };
	constexpr NameColors NAME_COLORS[] = {
		{ 0xFFFFFC40, 0xFFD6F264 }, // Red
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Magenta
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Rose
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Green
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Forest
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Cyan
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Periwinkle
		{ 0xFFFFFFFF, 0xFFDAE0EA }, // Steel
		{ 0xFFFFFFFF, 0xFFFFFFFF }, // Blue
		{ 0xFFFFFC40, 0xFFD6F264 }, // Orange
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Pine
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Chartreuse
		{ 0xFFF9A31B, 0xFFFA6A0A }, // Yellow
		{ 0xFFFFFFFF, 0xFFA6FCDB }, // Aqua
		{ 0xFF4AA1E8, 0xFF1062A5 }, // Sage
	};
	static_assert(sizeof(NAME_COLORS) / sizeof(NAME_COLORS[0]) == static_cast<size_t>(Demo::CardTemplate::Count),
		"one name colour per CardTemplate");
	constexpr D3DCOLOR COST_COLOR = 0xFFE3E6FF;
	// The "(_)" after the name was the same cool grey on every card.
	constexpr D3DCOLOR INPUT_COLOR = 0xFFB3B9D1;
	// A filled slot's "x" has to stand out from that grey: orange, except on the templates whose own
	// colours are orange or yellow. Cyan reads on orange but washes out against yellow, which wants
	// something dark - a saturated blue.
	constexpr D3DCOLOR FILLED_INPUT_COLOR = 0xFFFF9A1F;
	constexpr D3DCOLOR FILLED_INPUT_COLOR_ON_ORANGE = 0xFF22E6F0;
	constexpr D3DCOLOR FILLED_INPUT_COLOR_ON_YELLOW = 0xFF1B3FD6;
	constexpr D3DCOLOR OUTLINE_COLOR = 0xFF060608;

	// Component-wise multiply, the same way a sprite's colour modulates its texture.
	D3DCOLOR Modulate(D3DCOLOR a, D3DCOLOR b) {
		auto ch = [&](int shift) { return ((a >> shift) & 0xFF) * ((b >> shift) & 0xFF) / 255; };
		return D3DCOLOR_ARGB(ch(24), ch(16), ch(8), ch(0));
	}

	RECT TemplateRect(Demo::CardTemplate t) {
		int index = static_cast<int>(t);
		if (index < 0 || index >= static_cast<int>(Demo::CardTemplate::Count)) index = 0;
		if (index < LEFT_COLUMN_COUNT) {
			const int y = index * TEMPLATE_H;
			return { LEFT_COLUMN_X, y, LEFT_COLUMN_X + TEMPLATE_W, y + TEMPLATE_H };
		}
		const int y = (index - LEFT_COLUMN_COUNT + 1) * TEMPLATE_H;
		return { RIGHT_COLUMN_X, y, RIGHT_COLUMN_X + TEMPLATE_W, y + TEMPLATE_H };
	}
}

std::wstring Demo::CardDisplayNameFromSaveID(const std::string& saveID) {
	std::string id = saveID;
	const std::string suffix = "Card";
	if (id.size() > suffix.size() && id.compare(id.size() - suffix.size(), suffix.size(), suffix) == 0) {
		id.resize(id.size() - suffix.size());
	}
	return DX9GF::Utils::Utf8ToWide(id);
}

std::wstring Demo::CardInputSignature(size_t slots, size_t filled) {
	std::wstring signature = L"(";
	for (size_t i = 0; i < slots; ++i) {
		if (i > 0) signature += L',';
		signature += i < filled ? L'x' : L'_';
	}
	return signature + L")";
}

std::shared_ptr<Demo::CardFrame> Demo::CardFrame::Get(DX9GF::GraphicsDevice* graphicsDevice) {
	static std::weak_ptr<CardFrame> cache;
	auto frame = cache.lock();
	if (!frame || frame->graphicsDevice != graphicsDevice) {
		frame = std::make_shared<CardFrame>(graphicsDevice);
		cache = frame;
	}
	return frame;
}

Demo::CardFrame::CardFrame(DX9GF::GraphicsDevice* graphicsDevice) : graphicsDevice(graphicsDevice) {
	texture = std::make_shared<DX9GF::Texture>(graphicsDevice);
	texture->LoadTexture(L"assets/cardtemplates.png");
	leftCap = std::make_shared<DX9GF::StaticSprite>(texture.get());
	body = std::make_shared<DX9GF::StaticSprite>(texture.get());
	rightCap = std::make_shared<DX9GF::StaticSprite>(texture.get());
	orb = std::make_shared<DX9GF::StaticSprite>(texture.get());

	font = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_NAME, CARD_FONT_SIZE);
	fontSprite = std::make_shared<DX9GF::FontSprite>(font.get());
	topFont = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_TOP_NAME, CARD_FONT_SIZE);
	topSprite = std::make_shared<DX9GF::FontSprite>(topFont.get());
	bottomFont = std::make_shared<DX9GF::Font>(graphicsDevice, CARD_FONT_BOTTOM_NAME, CARD_FONT_SIZE);
	bottomSprite = std::make_shared<DX9GF::FontSprite>(bottomFont.get());
	fontSprite->SetText(std::wstring(L"0"));
	digitWidth = static_cast<float>(fontSprite->GetWidth());
}

float Demo::CardFrame::MeasureWidth(const std::wstring& name, const std::wstring& inputs, float scale) {
	fontSprite->SetText(name + inputs);
	const float textScale = scale / FONT_NATIVE_SCALE;
	const float textW = static_cast<float>(fontSprite->GetWidth()) * textScale;
	const float digitW = digitWidth * textScale;
	const float sheetW = LEFT_CAP_W + NAME_COST_GAP + DIGIT_ORB_GAP + ORB_SIZE + ORB_RIGHT_GAP + RIGHT_CAP_W;
	// Whole sheet pixels, so the body column scales by an exact multiple.
	return std::ceil((sheetW * scale + textW + digitW) / scale) * scale;
}

void Demo::CardFrame::DrawSlice(DX9GF::StaticSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
	RECT src, float x, float y, float scaleX, float scaleY, D3DCOLOR tint) {
	sprite.SetSrcRect(src);
	sprite.SetColor(tint);
	sprite.Begin();
	sprite.SetPosition(x, y);
	sprite.SetScale(scaleX, scaleY);
	sprite.Draw(camera, deltaTime);
	sprite.End();
}

void Demo::CardFrame::DrawLabel(DX9GF::FontSprite& sprite, const DX9GF::Camera& camera, unsigned long long deltaTime,
	const std::wstring& text, float x, float y, float scale, D3DCOLOR color, bool outlined, D3DCOLOR tint) {
	sprite.SetText(text);
	sprite.SetColor(color);
	sprite.SetOutline(outlined, Modulate(OUTLINE_COLOR, tint), scale);
	sprite.Begin();
	sprite.SetScale(scale / FONT_NATIVE_SCALE, scale / FONT_NATIVE_SCALE);
	sprite.SetPosition(x, y);
	sprite.Draw(camera, deltaTime);
	sprite.End();
}

void Demo::CardFrame::Draw(const DX9GF::Camera& camera, unsigned long long deltaTime, float x, float y,
	CardTemplate cardTemplate, const std::wstring& name, const std::wstring& inputs, size_t cost, float scale, D3DCOLOR tint) {
	const RECT t = TemplateRect(cardTemplate);
	const float width = MeasureWidth(name, inputs, scale);
	const float leftW = LEFT_CAP_W * scale;
	const float rightW = RIGHT_CAP_W * scale;
	const float bodyW = width - leftW - rightW;

	DrawSlice(*leftCap, camera, deltaTime, { t.left, t.top, t.left + LEFT_CAP_W, t.bottom },
		x, y, scale, scale, tint);
	// The body is one source column stretched across; its width in sheet pixels is the scale factor.
	DrawSlice(*body, camera, deltaTime, { t.left + BODY_SRC_X, t.top, t.left + BODY_SRC_X + 1, t.bottom },
		x + leftW, y, bodyW / 1.f, scale, tint);
	DrawSlice(*rightCap, camera, deltaTime, { t.right - RIGHT_CAP_W, t.top, t.right, t.bottom },
		x + width - rightW, y, scale, scale, tint);

	const float textScale = scale / FONT_NATIVE_SCALE;
	const float textY = y + TEXT_TOP * scale;

	// Two passes: each font only has its half of every letter, so together they make the whole name.
	const NameColors& nameColors = NAME_COLORS[static_cast<int>(cardTemplate)];
	DrawLabel(*topSprite, camera, deltaTime, name, x + leftW, textY, scale, Modulate(nameColors.top, tint), false, tint);
	DrawLabel(*bottomSprite, camera, deltaTime, name, x + leftW, textY, scale, Modulate(nameColors.bottom, tint), false, tint);

	fontSprite->SetText(name);
	const float nameW = static_cast<float>(fontSprite->GetWidth()) * textScale;
	// The signature is drawn in runs so each "x" can take its own colour. Glyph advances add up, so a
	// run's start is just the measured width of everything before it.
	D3DCOLOR filledColor = FILLED_INPUT_COLOR;
	if (cardTemplate == CardTemplate::Orange) filledColor = FILLED_INPUT_COLOR_ON_ORANGE;
	else if (cardTemplate == CardTemplate::Yellow) filledColor = FILLED_INPUT_COLOR_ON_YELLOW;
	float runX = x + leftW + nameW;
	size_t pos = 0;
	while (pos < inputs.size()) {
		size_t end = inputs[pos] == L'x' ? pos + 1 : inputs.find(L'x', pos);
		if (end == std::wstring::npos) end = inputs.size();
		const std::wstring run = inputs.substr(pos, end - pos);
		const bool filled = inputs[pos] == L'x';
		DrawLabel(*fontSprite, camera, deltaTime, run, runX, textY, scale,
			Modulate(filled ? filledColor : INPUT_COLOR, tint), false, tint);
		runX += static_cast<float>(fontSprite->GetWidth()) * textScale;
		pos = end;
	}

	const float orbX = x + width - rightW - (ORB_RIGHT_GAP + ORB_SIZE) * scale;
	DrawSlice(*orb, camera, deltaTime, ORB_SRC, orbX, y + (Height(scale) - ORB_SIZE * scale) / 2.f,
		scale, scale, tint);

	const float digitX = orbX - DIGIT_ORB_GAP * scale - digitWidth * textScale;
	DrawLabel(*fontSprite, camera, deltaTime, std::to_wstring(cost), digitX, textY, scale, Modulate(COST_COLOR, tint), true, tint);
}
